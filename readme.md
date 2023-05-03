# Smart Oven Project

A project by Enable, a student-led interest group under Nanyang Technology University’s Renaissance Engineering Programme focused on developing assistive technologies for various community partners.

[Product Demonstration]()
## Overview
Project information and prototype usage can be found [here](https://docs.google.com/document/d/1T6A8cfehCGghUUyGrNWb8jcuDFIxB7nuweHgq2GrgUs/edit?usp=sharing).

## Hardware
### Control Box
CAD files for the control box with its associated components are found in the [Hardware Directory](./Hardware/). Either PLA or PETG filament is suitable.

### Oven Modification
* Dismantle the oven and identify the wires connected to the oven's components (Heating Coil, Rotisserie, Fan, Bulb).
* Connect the thermal fuse to the wires connecting to the heating coil(s).
* Based on the Bulb's voltage requirements, attach the appropriate power resistor. (Power resistor is able to withstand the higher temperatures).
* Extend the wires to the 8 Channel Relay using the 14AWG wires.
* Heat shrink exposed ends.
* Reassemble the oven.

### High level Diagram:

![image](https://user-images.githubusercontent.com/77315991/235966263-edc31077-0570-401a-80f1-38e39e28b3be.png)


## Software
### Oven Control Logic
The software logic to control the various components of the oven was done with the [Arduino](https://www.arduino.cc/) programming language. The Arduino file can be found [here](./webSocket2/webSocket2.ino).

Other than interfacing with the various hardware components of the Oven, such as the thermoprobe and the OLED, the Arduino code also hosts the websocket server to process inputs from the mobile application frontend and send the neccesary information back to the application.

### User-Interfacing Frontend
The mobile application frontend was created using HTML, CSS and Javascript. The files can be found [here](./webSocket2/data/). These HTML, CSS and Javascript files were uploaded into the ESP32 via the Serial Peripheral Interface Flash File System (SPIFFS). 

The mobile application is responsible for displaying various information to the end-user, such as the currently selected setting, time left, and the current oven temperature. Users will also be able to set the temperature and duration for each oven functionality via the settings page.

## Electronics
### Wiring
* Connect the electrical components based on the wiring schema:
![REACH Enable Schematic pptx (1)](https://user-images.githubusercontent.com/77315991/235963603-66be0e25-3e35-4c61-a1a6-95d963be1af5.jpg)

## Bill of Materials
|Item|Quantity|Purpose|
|-|-|-|
|Conventional Oven|1|To be modified||
|ESP32|1|Micro-Controller|
|Micro USB Cable|1|To power and program micro-controller|
|PT100 Temperature Probe|1|To measure temperature changes|
|MAX31865 Module|1|To process the oven's temperature|
|OLED Display Module 0.96" |1|To display commands|
|Donut Board|1|To mount micro-controller|
|8 Chanel Relay|1|To control the oven's functionalities|
|220Ω Power Resistor|1|To reduce Voltage to the oven's bulb|
|250C 10A Thermal Fuse|2|Safety mechanism to prevent overheating|
|Wire Splitter|2|To split cables into parallel circuits|
|Heat Shrink Tube|As Required|To insulate exposed ends|
|14 AWG Wires|As Required|To connect |
|Jumper Cables|As Required|To connect electronic components|
|Cable Ties|As Required|For cable management|
