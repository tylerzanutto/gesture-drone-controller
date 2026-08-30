/*
  ESP32 Wireless Gesture-Controlled Robot Car - Receiver

  Receives glove control data using ESP-NOW and drives a 4WD robot car
  through a TB6612FNG motor driver.

  Mode 1:
    Forward/Back -> drive forward/backward
    Left/Right   -> turn left/right

  Mode 2:
    Motors stop.

*/

#include <WiFi.h>
#include <esp_now.h>

// =========================
// TB6612FNG motor pins
// =========================

const int AIN1 = 26;
const int AIN2 = 27;
const int PWMA = 25;

const int BIN1 = 14;
const int BIN2 = 12;
const int PWMB = 33;

const int STBY = 32;

// =========================
// Control settings
// =========================

const int maxMotorSpeed = 180; // 0-255. Change lower/higher if needed.
const int deadZone = 10;

// Temporary safety setting.
const bool TEST_ENABLE_WITHOUT_ARM = true;

// Stop motors if wireless data stops coming in
unsigned long lastReceiveTime = 0;
const unsigned long signalTimeout = 500; // milliseconds

// =========================
// Data structure from glove
// Must match transmitter exactly
// =========================

typedef struct ControlData {
  int mode;
  int forwardBack;
  int leftRight;
  int upDown;
  int yaw;
  bool armed;
} ControlData;

ControlData receivedData;

// =========================
// ESP-NOW receive callback
// =========================

void onDataReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  lastReceiveTime = millis();

  Serial.print("Mode: ");
  Serial.print(receivedData.mode);

  Serial.print(" | FB: ");
  Serial.print(receivedData.forwardBack);

  Serial.print(" | LR: ");
  Serial.print(receivedData.leftRight);

  Serial.print(" | Armed: ");
  Serial.println(receivedData.armed ? "YES" : "NO");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Motor direction pins
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  // Enable motor driver
  digitalWrite(STBY, HIGH);

  // New ESP32 PWM setup
  ledcAttach(PWMA, 1000, 8);
  ledcAttach(PWMB, 1000, 8);

  stopMotors();

  // ESP-NOW setup
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onDataReceive);

  Serial.println("Robot car receiver ready");
}

void loop() {
  // Failsafe: stop if wireless signal is lost
  if (millis() - lastReceiveTime > signalTimeout) {
    stopMotors();
    return;
  }

  // Temporary: allow testing before arm/disarm button is added
  bool controlEnabled = receivedData.armed || TEST_ENABLE_WITHOUT_ARM;

  if (!controlEnabled) {
    stopMotors();
    return;
  }

  // Only Mode 1 drives the car right now
  if (receivedData.mode != 1) {
    stopMotors();
    return;
  }

  int throttle = receivedData.forwardBack; // forward/back
  int steering = receivedData.leftRight;   // left/right

  if (abs(throttle) <= deadZone) {
    throttle = 0;
  }

  if (abs(steering) <= deadZone) {
    steering = 0;
  }

  /*
    Tank drive:

    throttle = forward/back
    steering = left/right

  */

  int leftSpeed = throttle + steering;
  int rightSpeed = throttle - steering;

  leftSpeed = constrain(leftSpeed, -100, 100);
  rightSpeed = constrain(rightSpeed, -100, 100);

  // Convert -100 to +100 into motor speed range
  leftSpeed = map(leftSpeed, -100, 100, -maxMotorSpeed, maxMotorSpeed);
  rightSpeed = map(rightSpeed, -100, 100, -maxMotorSpeed, maxMotorSpeed);

  Serial.print("Motor Output | LeftSpeed: ");
  Serial.print(leftSpeed);
  Serial.print(" | RightSpeed: ");
  Serial.println(rightSpeed);

  leftMotor(leftSpeed);
  rightMotor(rightSpeed);

  delay(50);
}

// =========================
// Motor functions
// =========================

void leftMotor(int speed) {
  speed = constrain(speed, -255, 255);

  if (speed > 0) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
    ledcWrite(PWMA, speed);
  } else if (speed < 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    ledcWrite(PWMA, -speed);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    ledcWrite(PWMA, 0);
  }
}

void rightMotor(int speed) {
  speed = constrain(speed, -255, 255);

  if (speed > 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
    ledcWrite(PWMB, speed);
  } else if (speed < 0) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
    ledcWrite(PWMB, -speed);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    ledcWrite(PWMB, 0);
  }
}

void stopMotors() {
  leftMotor(0);
  rightMotor(0);
}
