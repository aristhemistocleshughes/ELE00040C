///////////////////////////////////////////////////////////////////////////////
// TremorDetector.ino
//  Claude Code Generated Arduino Sketch for Tremor Detector for Nano 33 Sense
//  BLE Prototype. It was generated from the following source code:
//  - https://github.com/arduino-libraries/ArduinoBLE/
//       blob/master/examples/Peripheral/LED/LED.ino
//  - https://github.com/kosme/arduinoFFT/
//       blob/master/Examples/FFT_02/FFT_02.ino
//  - https://github.com/arduino-libraries/Arduino_LSM6DSOX/
//       blob/main/examples/SimpleAccelerometer/SimpleAccelerometer.ino
// Comments provide by Aris Hughes, adapted from the original code comments and
// expanded for clarity. No authorship is claimed for this file. See the source
// files above for license restrictions.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Arduino_BMI270_BMM150.h
//  Include IMU object that talks to the on-board BMI270 accelerometer/gyro.
// arduinoFFT.h
//  Brings in the ArduinoFFT template class used for the Fast Fourier
//  Transform.
// ArduinoBLE.h
//  Brings in the BLE, BLEService, BLECharacteristic, BLEDescriptor classes.
///////////////////////////////////////////////////////////////////////////////
#include <Arduino_BMI270_BMM150.h>
#include <arduinoFFT.h>
#include <ArduinoBLE.h>

///////////////////////////////////////////////////////////////////////////////
// SAMPLES: Number of samples per FFT window; MUST be a power of 2.
// SAMPLE_RATE: Sampling frequency in Hz, e.g  3-10s window.
// SAMPLE_US: Microseconds between samples, used by the pacing loop.
// MOTION_THRESHOLD: Stddev of acceleration (g) for hand considered as still.
// vReal: Real component buffer before/after FFT, magnitudes after FFT.
// vImag: Imaginary component buffer: zeroed before each FFT.
///////////////////////////////////////////////////////////////////////////////
const uint16_t SAMPLES = 256;
const float SAMPLE_RATE = 85.33;
const uint32_t SAMPLE_US = (uint32_t)(1000000.0 / SAMPLE_RATE);
const float MOTION_THRESHOLD = 0.03;
double vReal[SAMPLES];
double vImag[SAMPLES];

// Constructs the FFT engine. Template <double> = use double-precision floats.
// Parameters:
//  pointer to real buffer, pointer to imag buffer, sample count, sample rate.
//  The object stores these for use by later calls (windowing/compute/etc.).
ArduinoFFT<double> FFT(vReal, vImag, SAMPLES, SAMPLE_RATE);

// Declares a BLE service with a 128-bit custom UUID; a "service" is a logical
// group of related characteristics.
BLEService tremorService("19B10000-E8F2-537E-4F6C-D104768A1214");

// declares a string-typed characteristic.
// Param 1: UUID. Param 2: properties - BLERead lets a client poll, BLENotify
//          lets the device push updates.
// Param 3: max value length in bytes (20 is enough for "12.34 Hz").
BLEStringCharacteristic tremorFreq("19B10001-E8F2-537E-4F6C-D104768A1214",
                                   BLERead | BLENotify, 20);

// 0x2901 is the SIG-standard "Characteristic User Description" descriptor
// - a label for BLE apps to show next to the value.
BLEDescriptor userDesc("2901", "Tremor Freq (Hz)");

// 7-byte payload for the Presentation Format descriptor:
//   byte 0 (0x19)    = format code -> UTF-8 string
//   byte 1 (0x00)    = exponent (none)
//   bytes 2-3 (2722) = SIG unit code for Hertz (little-endian)
//   byte 4 (0x01)    = namespace = Bluetooth SIG
//   bytes 5-6        = description (unused)
const uint8_t pfBytes[7] = {0x19, 0x00, 0x22, 0x27, 0x01, 0x00, 0x00};

// 0x2904 is the SIG-standard "Characteristic Presentation Format" descriptor.
// Param 1: UUID. Param 2: pointer to the data bytes. Param 3: number of bytes.
// Tells clients to display the characteristic value as UTF-8 text rather than
// raw hex.
BLEDescriptor presentationFormat("2904", pfBytes, sizeof(pfBytes));


///////////////////////////////////////////////////////////////////////////////
// setup()
// -------
// Parameters : none (Arduino framework calls it once at boot).
// Returns    : void.
// Purpose    :
//  One-time initialisation. Brings up the USB serial link, the IMU, and the
//  BLE radio; builds the GATT structure (descriptors -> characteristic ->
//  service -> BLE stack); writes an initial value; then starts advertising
//  so phones can discover the device.
//  Halts if IMU.begin() or BLE.begin() returns false.
///////////////////////////////////////////////////////////////////////////////
void setup()
{

    // opens the USB CDC serial port at 115200 baud (must match the host)
    Serial.begin(115200);

    // 2-second pause so USB enumeration + IMU power-up complete before use
    delay(2000);

    if (!IMU.begin())
    {
        // IMU.begin() returned false as failure
        // print diagnostic so the user/log sees why we're stuck
        Serial.println("IMU FAILED"); 

        // infinite empty loop as nothing useful possible withoutthe sensor
        while (1)
            ; 
    }
    // log success path so we can confirm initialisation order
    Serial.println("IMU OK"); 

    if (!BLE.begin())
    {
        // BLE.begin() returns true on success; same pattern as IMU
        Serial.println("BLE FAILED");
        while (1)
            ; // halt again — no Bluetooth means no use
    }

    // sets the human-readable name shown in phone scan lists
    BLE.setLocalName("Tremor-V2");

    // service UUID in advertising packets so clients can filter by it
    BLE.setAdvertisedService(tremorService);

    // attaches the label descriptor to the characteristic
    tremorFreq.addDescriptor(userDesc);
    
    // attaches the format descriptor 
    // (must be added BEFORE the char is added to its service)
    tremorFreq.addDescriptor(presentationFormat);

    // registers the characteristic inside its service
    tremorService.addCharacteristic(tremorFreq);

    // registers the service with the BLE stack — now part of the GATT table
    BLE.addService(tremorService);

    // seeds the initial value so newly-connected clients see something
    // immediately
    tremorFreq.writeValue("0.00 Hz");

    // begins broadcasting connectable advertising packets every ~625 ms
    BLE.advertise();

    // log so the operator knows we're now discoverable
    Serial.println("BLE advertising");
}

///////////////////////////////////////////////////////////////////////////////
// lastHeartbeat:
//  millis() timestamp of the last "alive" message; used for the 2s heartbeat
// acquisitionEnabled:
//  gate flag toggled by "start"/"stop" commands; controls whether FFT runs
// lastReading: 
//  most recently published reading; returned by "freq"/"status" commands
///////////////////////////////////////////////////////////////////////////////
unsigned long lastHeartbeat = 0;
bool acquisitionEnabled = true;
String lastReading = "0.00 Hz";

///////////////////////////////////////////////////////////////////////////////
// handleSerialCommand()
// ---------------------
// Parameters
// none - reads bytes directly from the global Serial object.
// Returns
//  void - side effects only (mutates acquisitionEnabled, writes to Serial).
// Purpose    : 
//  Non-blocking command parser. Called every loop iteration. 
//  Reads one newline-terminated line from USB serial (if any is waiting), 
//  normalises it, and dispatches.
//  This is the "backend" of the desktop PowerShell terminal - the terminal
//  writes commands over USB and this function executes them on the device.
// Commands
//              start  -> set acquisitionEnabled = true
//              stop   -> set acquisitionEnabled = false
//              freq   -> print lastReading
//              status -> print acquisitionEnabled + lastReading
//              other  -> echo "[cmd] unknown: ..." so typos are visible
///////////////////////////////////////////////////////////////////////////////
void handleSerialCommand()
{
    // Serial.available() returns the number of waiting bytes; 
    // 0 -> nothing to do, return early
    if (!Serial.available())
        return;
    
    // reads bytes into a String up to (but not including) the newline; 
    // blocks briefly
    String cmd = Serial.readStringUntil('\n');

    // String::trim() removes leading/trailing whitespace IN-PLACE
    cmd.trim();

    // String::toLowerCase() converts in-place
    // - makes comparisons case-insensitive
    cmd.toLowerCase();

    if (cmd == "start")
    {                                                
        // String::operator== does a character-by-character compare
        // resume the FFT loop on the next iteration
        acquisitionEnabled = true;

        // ack the command back to the terminal
        Serial.println("[cmd] acquisition started");
    }
    else if (cmd == "stop")
    {
        // pause the FFT loop (BLE connection stays alive)
        acquisitionEnabled = false;
        Serial.println("[cmd] acquisition paused");
    }
    else if (cmd == "freq")
    {
         // print() does not add a newline; lets us concatenate values
        Serial.print("[cmd] last: ");
        // println() adds CR+LF so the terminal sees a complete line
        Serial.println(lastReading);
    }
    else if (cmd == "status")
    {
        Serial.print("[cmd] acquisition=");
         // ternary: prints "ON" or "OFF" depending on the flag
        Serial.print(acquisitionEnabled ? "ON" : "OFF");
        Serial.print(" last=");
        Serial.println(lastReading);
    }
    else if (cmd.length() > 0)
    {
        // String::length() returns the character count;
        // protects against empty lines
        // echo back any unrecognised non-empty input
        Serial.print("[cmd] unknown: ");
        Serial.println(cmd);
    }
}

///////////////////////////////////////////////////////////////////////////////
// loop()
// ------
// Parameters
//  none (Arduino framework calls it repeatedly forever after setup()).
// Returns
//  void.
// Purpose
//  The main runtime. Two phases:
//  1) When NO central is connected: poll for serial commands and emit a 
//     heartbeat message every two seconds.
//  2) When a central IS connected: repeatedly sample the IMU for SAMPLES
//     values, remove the DC (gravity) component, gate on motion stddev, run
//     FFT if moving, compute the dominant frequency, format it as a string,
//     store it, and notify over BLE so the connected phone receives the new
//     value.
//  Key trick:
//  BLE.poll() is called inside the sampling loop every 8 samples so that the
//  BLE stack keeps servicing connection events during the ~3 s sample 
//  window. Without this, iOS sees the device as unresponsive and drops the
//  connection (taking the subscription with it).
///////////////////////////////////////////////////////////////////////////////
void loop()
{
    // run the command parser once per iteration so the terminal stays
    // responsive
    handleSerialCommand();

    if (millis() - lastHeartbeat > 2000)
    { 
        // millis() returns ms since boot; subtraction handles uint
        //rollover correctly
        Serial.println("[heartbeat] waiting for BLE central...");
        lastHeartbeat = millis(); // reset the heartbeat clock
    }

    // non-blocking: returns a connected client if any, otherwise an "empty"
    // BLEDevice
    BLEDevice central = BLE.central();

    if (central)
    {
        // BLEDevice::operator bool() is true when a connection exists
        Serial.print("Connected to: ");

        // central.address() returns the central's MAC as a String
        //(e.g. "79:11:68:02:c9:e1")
        Serial.println(central.address());

        while (central.connected())
        {
            // central.connected() returns true while the link is up;
            // loop until disconnect
            // allow start/stop/freq/status commands during the connection too
            handleSerialCommand();

            if (!acquisitionEnabled)
            {
                // command-driven pause
                delay(100); // sleep briefly to avoid burning CPU while paused
                continue;   // skip the FFT cycle and re-check next iteration
            }

            Serial.println("Starting collection...");
            // declared here (not in the for-loop) to avoid re-creating each
            // iteration
            float ax, ay, az;

            for (uint16_t i = 0; i < SAMPLES; i++)
            {
                // collect SAMPLES (256) acceleration samples at SAMPLE_RATE Hz
                // micros() = us since boot; used to time this sample slot
                // precisely
                uint32_t t = micros();

                // spin until the IMU reports a fresh sample is ready
                while (!IMU.accelerationAvailable())
                    ;                             
                
                    // reads X/Y/Z accelerations into the variables
                    //BY REFERENCE (output params)
                    IMU.readAcceleration(ax, ay, az);

                // magnitude of the 3-axis vector = orientation-independent
                // acceleration
                vReal[i] = sqrt(ax * ax + ay * ay + az * az);
                
                // imaginary part must be 0 because our input is purely real
                vImag[i] = 0.0;

                // bitwise: i mod 8 == 0; every 8 samples (~94 ms)
                // service BLE events
                if ((i & 0x07) == 0)
                {
                    // BLE.poll() processes incoming BLE events so iOS
                    // doesn't timeout the link
                    BLE.poll();
                }

                // busy-wait until SAMPLE_US us have elapsed - keeps sample
                // rate accurate
                while (micros() - t < SAMPLE_US)
                    ; 
            }

            double mean = 0; // accumulator for the mean of vReal
            for (uint16_t i = 0; i < SAMPLES; i++)
                mean += vReal[i]; // sum all samples

            // divide by count -> arithmetic mean (~ 1 g from gravity)
            mean /= SAMPLES;      
            
             // subtract mean from every sample = DC removal
             for (uint16_t i = 0; i < SAMPLES; i++)
                vReal[i] -= mean;

            double sumSq = 0; // accumulator for sum of squares

            // sum of squared deviations (mean is already 0)
            for (uint16_t i = 0; i < SAMPLES; i++)
                sumSq += vReal[i] * vReal[i];
            
            // standard deviation = sqrt(variance); units = g
            double stddev = sqrt(sumSq / SAMPLES);

            // fixed-size C string buffer for the formatted output
            char buffer[20];

            if (stddev < MOTION_THRESHOLD)
            {
                // gate: too little motion -> don't run FFT, report 0 Hz
                // snprintf(dest, max_size, format, ...) -> writes "0.00 Hz"
                // into buffer, never exceeding 20 bytes (NUL-safe)
                snprintf(buffer, sizeof(buffer), "0.00 Hz");
                
                Serial.print("Stationary (stddev=");
                // print(value, digits). second arg = decimal places for floats
                Serial.print(stddev, 4);
                Serial.println(")");
            }
            else
            {
                // apply a Hamming window IN-PLACE to vReal.
                // Param 1:
                //  window function (Hamming reduces spectral leakage from
                //  window edges).
                // Param 2: direction (Forward = pre-FFT; Reverse would undo).
                FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);

                // perform FFT IN-PLACE on vReal/vImag. After this, both
                // buffers hold complex frequency-domain values.
                // Param: direction (Forward = time->frequency; 
                //                    Reverse = frequency->time).
                FFT.compute(FFTDirection::Forward);

                // collapses (re, im) pairs into magnitudes IN-PLACE: 
                // vReal[i] = sqrt(vReal[i]^2 + vImag[i]^2). 
                // vImag becomes meaningless after this.
                FFT.complexToMagnitude();

                // tracker for the largest magnitude found so far
                double peakMag = 0;

                // tracker for which bin index that magnitude is in
                // (start at 1 to skip DC)
                uint16_t peakBin = 1;
                for (uint16_t i = 1; i < SAMPLES / 2; i++)
                {
                    // scan first half only - above Nyquist (SAMPLES/2) is
                    //mirrored noise
                    if (vReal[i] > peakMag)
                    {                       // new winner?
                        peakMag = vReal[i]; // record the magnitude
                        peakBin = i;        // record the bin index
                    }
                }

                // bin -> frequency conversion: 
                // each bin represents (SAMPLE_RATE / SAMPLES) Hz;
                //peakBin x that = peak frequency in Hz.
                double dominantHz = (peakBin * SAMPLE_RATE) / SAMPLES;

                // formats the result as e.g. "4.80 Hz" into the buffer;
                // %.2f = float with 2 decimal places.
                snprintf(buffer, sizeof(buffer), "%.2f Hz", dominantHz);

                Serial.print("Dominant frequency: ");
                Serial.print(dominantHz, 2);
                Serial.print(" Hz (stddev=");
                Serial.print(stddev, 4);
                Serial.println(")");
            }

            // converts the C-string buffer into an Arduino String for
            // command replies
            lastReading = String(buffer);
             
            // updates the BLE characteristic; subscribed clients receive
            // a notification automatically
            tremorFreq.writeValue(buffer);
        }

        // reached when central.connected() returns false -> connection lost
        Serial.println("Disconnected");
    }
}