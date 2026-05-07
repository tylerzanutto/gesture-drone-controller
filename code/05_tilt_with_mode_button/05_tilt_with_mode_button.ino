#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Pins
const int modeButtonPin = 18;

// Control settings
const int deadZone = 10;
const int minControl = -100;
const int maxControl = 100;

// Mode/button variables
int mode = 1; // 1 = movement, 2 = height/yaw
int lastButtonState = HIGH;

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(modeButtonPin, INPUT_PULLUP);

  Wire.begin(21, 22);

  Serial.println("Starting tilt + mode button test...");


  // Check MPU6050 connection
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found.");
    while (1) {
      delay(10);
    }
  }

  Serial.println("MPU6050 connected!");
  Serial.println("Mode 1: Movement");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  
  // Read button
  int buttonState = digitalRead(modeButtonPin);

  // Detect one button press, not holding
  if (lastButtonState == HIGH && buttonState == LOW) {
    if (mode == 1) {
      mode = 2;
      Serial.println("Switched to Mode 2: Height/Yaw");
    } else {
      mode = 1;
      Serial.println("Switched to Mode 1: Movement");
    }

    delay(250); // simple debounce
  }

  lastButtonState = buttonState;

  // Read MPU6050
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  int xTilt = accel.acceleration.x * 100;
  int yTilt = accel.acceleration.y * 100;

  int tiltForwardBack = map(xTilt, -900, 900, minControl, maxControl);
  int tiltLeftRight   = map(yTilt, -900, 900, minControl, maxControl);

  tiltForwardBack = constrain(tiltForwardBack, minControl, maxControl);
  tiltLeftRight   = constrain(tiltLeftRight, minControl, maxControl);

  if (abs(tiltForwardBack) <= deadZone) {
    tiltForwardBack = 0;
  }

  if (abs(tiltLeftRight) <= deadZone) {
    tiltLeftRight = 0;
  }

  // Drone-style output values
  int forwardBack = 0;
  int leftRight = 0;
  int upDown = 0;
  int yaw = 0;

  if (mode == 1) {
    // Movement mode
    forwardBack = tiltForwardBack;
    leftRight = tiltLeftRight;
  } else {
    // Height/yaw mode
    upDown = tiltForwardBack;
    yaw = tiltLeftRight;
  }

  Serial.print("Mode: ");
  Serial.print(mode);

  Serial.print(" | Forward/Back: ");
  Serial.print(forwardBack);

  Serial.print(" | Left/Right: ");
  Serial.print(leftRight);

  Serial.print(" | Up/Down: ");
  Serial.print(upDown);

  Serial.print(" | Yaw: ");
  Serial.println(yaw);

  delay(150);
}
