# Smart Oven Project

A project by Enable, a student-led interest group under Nanyang Technology University’s Renaissance Engineering Programme focused on developing assistive technologies for various community partners.

[Product Demonstration]()
## Overview
Project information and prototype usage can be found [here](https://docs.google.com/document/d/1T6A8cfehCGghUUyGrNWb8jcuDFIxB7nuweHgq2GrgUs/edit?usp=sharing).
## Bill of Materials

## Hardware
### Control Box
CAD files for the control box with its associated components are found in the Hardware Directory. Either PLA or PETG filament is suitable.

### Oven Hacking 

## Software
### Oven Control Logic
The software logic to control the various components of the oven was done with the [Arduino](https://www.arduino.cc/) programming language. The Arduino file can be found [here](./webSocket2/webSocket2.ino).

Other than interfacing with the various hardware components of the Oven, such as the thermoprobe and the OLED, the Arduino code also hosts the websocket server to process inputs from the mobile application frontend and send the neccesary information back to the application.

### User-Interfacing Frontend
The mobile application frontend was created using HTML, CSS and Javascript. The files can be found [here](./webSocket2/data/). These HTML, CSS and Javascript files were uploaded into the ESP32 via the Serial Peripheral Interface Flash File System (SPIFFS). 

The mobile application is responsible for displaying various information to the end-user, such as the currently selected setting, time left, and the current oven temperature. Users will also be able to set the temperature and duration for each oven functionality via the settings page.

## Electronics
### Wiring
### ESP32 Programming


