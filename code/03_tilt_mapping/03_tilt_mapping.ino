#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Create a dead zone to remove small accidental movements
const int deadZone = 10;

// These values are the output range for drone-style control
const int minControl = -100;
const int maxControl = 100;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(21, 22);

  // Check MPU6050 connection
  Serial.println("Starting MPU6050 tilt mapping...");

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found. Check wiring.");
    while (1) {
      delay(10);
    }
  }

  Serial.println("MPU6050 connected!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  /*
    accel.acceleration.x and accel.acceleration.y are in m/s^2.
    When tilted, they usually range around -9.8 to +9.8.
    Multiply by 100 so map() can work nicely with integers.
  */

  int xTilt = accel.acceleration.x * 100;
  int yTilt = accel.acceleration.y * 100;

  // Map tilt values to control range -100 to +100
  int forwardBack = map(xTilt, -900, 900, minControl, maxControl);
  int leftRight   = map(yTilt, -900, 900, minControl, maxControl);

  // Keep values inside -100 to +100
  forwardBack = constrain(forwardBack, minControl, maxControl);
  leftRight   = constrain(leftRight, minControl, maxControl);

  // Apply dead zone
  if (abs(forwardBack) < deadZone) {
    forwardBack = 0;
  }

  if (abs(leftRight) < deadZone) {
    leftRight = 0;
  }

  Serial.print("Forward/Back: ");
  Serial.print(forwardBack);

  Serial.print(" | Left/Right: ");
  Serial.print(leftRight);

  Serial.print(" | Raw X: ");
  Serial.print(accel.acceleration.x);

  Serial.print(" | Raw Y: ");
  Serial.println(accel.acceleration.y);

  delay(100);
}
