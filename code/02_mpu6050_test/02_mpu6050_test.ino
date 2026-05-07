#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(21, 22);

  Serial.println("Starting MPU6050 test...");

  //check MPU6050 connection
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found.");
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

  //get accel in each direction
  Serial.print("Accel X: ");
  Serial.print(accel.acceleration.x);
  Serial.print(" | Accel Y: ");
  Serial.print(accel.acceleration.y);
  Serial.print(" | Accel Z: ");
  Serial.print(accel.acceleration.z);

  Serial.print(" || Gyro X: ");
  Serial.print(gyro.gyro.x);
  Serial.print(" | Gyro Y: ");
  Serial.print(gyro.gyro.y);
  Serial.print(" | Gyro Z: ");
  Serial.println(gyro.gyro.z);

  delay(200);
}
