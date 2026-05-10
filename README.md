# ESP32 Glove Drone Controller

This project is a glove-based gesture controller for a drone using an ESP32 and an MPU6050 motion sensor. The goal is to control drone movement using hand tilt, with buttons for mode switching and arm/disarm control.

The current plan is to use the glove as a wireless controller. A second ESP32 may later be used near a modified drone remote/controller to receive the glove data and convert it into joystick-style control signals.

## Project Goal

The final glove controller will include:

- ESP32 microcontroller
- MPU6050 accelerometer/gyroscope
- Mode-changing button
- Arm/disarm button
- Power switch for the glove
- LED indicator(s) for mode and/or armed status
- Wireless communication to a second ESP32 or drone controller system

## Control Design

The glove will use two control modes.

| Glove Action | Mode 1: Movement | Mode 2: Height/Yaw |
|---|---|---|
| Tilt forward/back | Drone forward/back | Drone up/down |
| Tilt left/right | Drone left/right | Drone yaw left/right |
| Mode button | Switch modes | Switch modes |
| Arm/disarm button | Arm/disarm drone | Arm/disarm drone |

The arm/disarm button is planned to use a long press for safety so the drone cannot be armed accidentally.

## Current Progress

### Completed

- Set up ESP32-WROOM-32 in Arduino IDE
- Connected the MPU6050 to the ESP32 using I2C
- Confirmed accelerometer and gyroscope values print in Serial Monitor
- Mapped MPU6050 tilt values to a control range from -100 to +100
- Added a dead zone so flat/near-flat readings return 0
- Added and tested a 4-pin tactile push button for mode switching
- Confirmed the button works using `INPUT_PULLUP`
- Started combining tilt mapping with mode switching

### Wireless Glove Demo

- Combined MPU6050 tilt mapping with ESP-NOW wireless communication
- Set up the glove ESP32 as a wireless transmitter
- Set up a second ESP32 as a receiver/display unit
- Confirmed real tilt values are sent wirelessly from the glove to the receiver
- Confirmed mode button switches between movement mode and height/yaw mode over wireless

### Current Stage

The current working stage is:
code/07_wireless_glove_controller
