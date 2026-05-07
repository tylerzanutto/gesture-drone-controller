# gesture-drone-controller

This project is a glove-based drone controller using an ESP32 and MPU6050 motion sensor. The goal is to map hand tilt to drone movement commands and add buttons for mode switching and arm/disarm control.

## Current Progress

- Set up ESP32-WROOM-32 in Arduino IDE
- Connected MPU6050 using I2C
- Confirmed accelerometer and gyroscope values print in Serial Monitor

## Planned Features

- Read hand tilt using the MPU6050
- Convert tilt values into movement commands
- Add a mode-switching button
- Add an arm/disarm button
- Add LED indicators for mode and armed state
- Interface the glove controller with a modified drone controller

## Hardware Used

- ESP32-WROOM-32 development board
- MPU6050 accelerometer/gyroscope module
- Breadboard
- Jumper wires
- USB-C cable

## Software

- Arduino IDE
- ESP32 board package by Espressif Systems
- Adafruit MPU6050 library
- Adafruit Unified Sensor library
- Adafruit BusIO library

## Setup Notes

The ESP32 board required the Silicon Labs CP210x USB-to-UART driver before Windows detected the board as a COM port.

The board was selected in Arduino IDE as:
ESP32 Dev Module
