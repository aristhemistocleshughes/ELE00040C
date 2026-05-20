
# ELE00040C Requirements Analysis

## Introduction

This document analyses the product and project requirements from the ELE00040C project specification document [1]. It will form the basis of the product and prototype specifications.

Text for a requirement is reproduced from [1] and formatted in this document as follows:

*“Text formatted like this has been copied from the original specification document [1].”*

i.e. in quoted italics. The requirements is then decomposed and elaborated in normal text.

References to sections refers to a number section in the original specification document e.g. "Section 2.0 Assignment Scenario" refers to Section 2.0 in the specification.

## Section 1.0 Requirements

From [1]:

*“Challenge 1 is the specification, design, and construction of a wearable medical device. The device must be able to measure a range of tremors of differing frequencies and characteristics. The tremors being measured will be associated with Parkinson’s disease and another commonly misdiagnosed neurological disease. Using these measurements for Rest, Postural, Kinetic and Action tremors, as appropriate, the device should be able to differentiate between Parkinson’s and one of the following commonly misdiagnosed conditions; Chorea/Dyskinesias or Essential tremors.”*

Scope Requirements:
1. The device is wearable e.g. like a watch on the patients wrist [MUST].
1. The device records mechanical oscillations in 3D space (tremors) of differing frequencies e.g using an accelerometer [MUST].
1. The device measures different kinds of tremors:
    1. Rest Tremors (todo: define) [SHOULD].
    1. Postural Tremors (todo: define) [SHOULD].
    1. Kinetic Tremors (todo: define) [SHOULD].
    1. Action Tremors (todo: define) [SHOULD].
1. The device differentiates between:
    1. Parkinson’s tremors [MUST].
    1. Chorea/Dyskinesias (todo: define) or Essential (todo: define) tremors [MUST].

From [1]:

*“Challenge 2 focuses on testing your device. To do this you will need to simulate a characteristic
tremor that can be measured using your device. A mechanical tremor actuator will be designed
and manufactured. This process will include different aspects of computing, electronic and
mechanical design.”*

Scope Requirements:
1. The device under test will be attached to a mechanical tremor actuator (test platform or test jig) generating mechical oscillations mimicing tremors [MUST].
1. The test jig oscillations are displacements in 3D space [SHOULD].
1. Possible test jig oscillations frequencies are characteristic of the tremor types [SHOULD].
1. Possible test jig oscillations frequencies are also not characteristic tremor types to test for false positives [SHOULD].


## Section 2.0 Assignment Scenario Requirements

Project Requirements:

1. *“… produce an initial version of the wearable device and demonstrate that it meets the appropriate technical specifications...”*
1. *“... should perform an initial literature search and use theinformation to create a set of technical specifications for the device...”*
1. *“...consider where within the appropriate medical regulatory framework your device sits…”*
1. *“… develop a plan for how you would take your prototype device to market ...”*
1. *“...must keep the costs low and your prototype and testing method must cost £100 or less to produce...”*
1. *“… employ different components (such as an alternative microcontroller or accelerometer) this must be justified (for example by reference to specific technical specifications) ...”*


## Section 2.1 Your project team [1]

1. *“… you should not think of this assignment as several individual projects – but rather as one coordinated activity, spread out between multiple people, using common code, designs and equipment.”*


## Section 2.2 Technical Research

1. *“Review appropriate literature and research to:"*
    1. *"Determine the characteristics of tremors associated with Parkinson’s Disease, essential tremor or Dystonia."*
    1. *"Identify the needs of the user in order to justify design choices."*
    1. *"Determine an appropriate mechanical mechanism to mimic the characteristics of the tremors identified in your research.”*
1. *“Learn how to analyse the data produced by the accelerometer to characterise the frequency and motion of the wearable device.”*


## Section 2.3 Supervision and Document Control

1. *“You should schedule meetings with your mentor as appropriate, I suggest scheduling them in weeks 2, 3, 5, 7, 8 and 10”*
1. *“you should prepare a project charter”*
1. *“decided on a team structure”*
1. *“expand the outline gantt chart”*
1. *“review your log and assess your progress against the Gantt chart”*
1. *“Complete the team log each week”*
1. *“provide evidence of your contribution to the project in the team log, this will be referenced by you in your project specification and final report” [MUST]*
1. *“Team log and outline Gantt chart templates are provided in your team's google drive folder”*
1. *“report should include minutes of any group meetings”*


## Section 2.4 The Impact of Engineering on Society [1] 

1. *”… address … the societal implications of your innovations ...[considering] the five key societal engineering themes that have been defined by the UK Engineering Council:"*
    1. *"Sustainability,"*
    1. *"Ethics,"*
    1. *"Equality/Inclusion,"*
    1. *"Risk, and"*
    1. *"Security”*


## Section 3 Checklist of Individual Tasks

### Section 3.1 Ideation and Planning

1. *"Define the characteristics of the tremors you will be measuring. Remember, you need to differentiate between at least two types of tremors."*
2. Determine a method for measuring tremors that can be housed in a wearable device, taking into account the requirements of the user.
3. Consider how and what data will be recorded.
4. Determine a mechanical mechanism that can mimic the characteristic of the tremors.
5. Consider opportunities for innovation and specialisation.
6. Equality is an important element of the design process. In designing your device are you considering all stakeholders and their needs?
7. Consider the security of the data collected by your device.
8. Produce a project plan that breaks down the work to be completed, construct an appropriate timeline of activities, plan identifies and mitigate risks.
9. What impact does your idea have on society?
10. Determine whether there are any ethical considerations that need addressing in your project.
11. Complete the ethics form on the vle; this will aidyou in identifying any areas of ethics to address.
12. Does your project plan equitably distribute the work between members of the team, and addresses and manages risks to the project.


### Section 3.2 Product design

1. Design the electronic components for the wearable device. Don’t forget to include a suitable method for:
    1. collecting tremor data.
    1. recording tremor data.
1. Design a suitable housing and method for mounting the wearable device.
1. Design and implement the logic for the microcontroller.
1. Design a mechanical actuator that can be used to mimic tremor characteristics. A prototype stand is provided that can be used to house your actuator, feel free to use this or design your own if you have the time available.
1. What impact does your design have on society?
1. Identify any constraints for your design, these can include environmental and sustainability limitations; ethical, security and risk issues; intellectual property.


### Section 3.3 Product Manufacturing

1. *"Determine the most appropriate method for manufacturing your individual components."*
    1. *"Try to strike a balance between cost and desired finish, e.g. 3D printing is available but can be slow and expensive. Laser cutting is quick and much cheaper."*
1. *"Determine appropriate materials for each component."*
1. *"Produce CAD files for all the physical components that can be used by the 3D printer or the laser cutter."*
1. *"Consider the use of CAD to test the mechanical design prior to the construction of your tremor generator."*
1. *"You will be introduced to the manufacturing methods available in the first workshop session in week 1. However, if you are unsure which processes to employ or what is available then please talk with the workshop technical staff or your module coordinator."*
1. *"What impact do your manufacturing choices have on society?■​ Consider the sustainability of your material choices."*


### Section 3.4 Testing and Refinement

1. *"Design and perform tests to demonstrate that the wearable device meets the specification."*
1. *"Learn the appropriate statistical methods for analysing the data (a lecture will be provided on this subject prior to this state of the project starting)."*
1. *"Collect and analyse test data for the wearable device."*
1. *"Refine the design of the tremor generator or wearable device based on the results of the tests."*


### Section 3.5 Documentation and presentation:

1. *"In your team log, document your project's management plan, design process, technical specifications, and any innovations or specialisations."*
1. *"Combine all of the work performed whilst planning into your initial group report (specification) and the initial group presentation."*
1. *"Prepare a final presentation demonstrating your wearable device and show that it meets the specifications."*
1. *"Write your comprehensive individual report."*


# References

1. ELE00040C_Engineering_project_1_Assessment_Documentation_2025-26_Live.pdf
2. sensors-20-05817_Parkinsons_Disease_Tremor_Detection_in_the_Wild_Using_Wearable_Accelerometers.pdf
3. sensors-21-00981_A-WEAR_Bracelet_for_Detection_of_Hand_Tremor.pdf
4. sensors-25-04313_Smart_Watch_Sensors_for_Tremor_Assessment_in_Parkinsons.pdf


