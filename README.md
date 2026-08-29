# ESP32 Wireless Gesture-Controlled Robot

This project is a wireless gesture-controlled robot system using an ESP32 and an MPU6050 motion sensor mounted on a glove. The glove reads hand tilt and button inputs, then sends control data wirelessly using ESP-NOW.

The project originally started as a glove-based drone controller. After completing the glove sensor, mode button, and wireless ESP-NOW stages, the project direction was updated to control a 4WD robot car instead. This keeps the same core embedded control system while making the final robot easier and safer to complete.

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

The glove currently uses two control modes.

| Glove Action | Mode 1: Robot Movement | Mode 2: Future/Alternate Control |
|---|---|---|
| Tilt forward/back | Car forward/back | Future feature |
| Tilt left/right | Car left/right turning | Future feature |
| Mode button | Switch modes | Switch modes |
| Arm/disarm button | Enable/disable motor control | Enable/disable motor control |

The arm/disarm button is planned to use a long press for safety so the robot cannot move accidentally.

### Robot Car Progress

- Assembled a 4WD acrylic robot car chassis
- Soldered wires to all four TT DC motors
- Grouped the motors by side: left front/rear and right front/rear
- Connected the robot car to a TB6612FNG motor driver
- Tested left and right motor groups using the receiver ESP32
- Confirmed the right motors spin correctly
- Reversed the left motor direction in code so both sides move forward correctly

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
code/08_motor_driver_test
