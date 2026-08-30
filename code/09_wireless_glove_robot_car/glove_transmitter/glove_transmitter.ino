
/*
  ESP32 Wireless Gesture-Controlled Robot Car - Glove Transmitter

  Reads MPU6050 tilt values and a mode button, then sends robot control
  values wirelessly to the car ESP32 using ESP-NOW.

  Mode 1: Robot Movement
    Tilt forward/back -> Forward/Back
    Tilt left/right   -> Left/Right

  Mode 2: Motors stop on car side for now
*/

#include <Wire.h>
#include <WiFi.h>
#include <esp_now.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Broadcast address sends to nearby ESP32 receivers
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Pins
const int modeButtonPin = 18;

// Control settings
const int deadZone = 15;
const int minControl = -100;
const int maxControl = 100;

// Mode/button variables
int mode = 1; // 1 = movement, 2 = stop
int lastButtonState = HIGH;

// Data structure sent to receiver
typedef struct ControlData {
  int mode;
  int forwardBack;
  int leftRight;
  int upDown;
  int yaw;
  bool armed;
} ControlData;

ControlData controlData;

void onDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(modeButtonPin, INPUT_PULLUP);

  // MPU6050 setup
  Wire.begin(21, 22); // SDA = D21, SCL = D22

  Serial.println("Starting wireless glove transmitter...");

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

  // ESP-NOW setup
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add ESP-NOW peer");
    return;
  }

  Serial.println("Glove transmitter ready");
  Serial.println("Mode 1: Robot Movement");
}

void loop() {
  // Read mode button
  int buttonState = digitalRead(modeButtonPin);

  // Detect button press
  if (lastButtonState == HIGH && buttonState == LOW) {
    if (mode == 1) {
      mode = 2;
      Serial.println("Switched to Mode 2: Stop/Future Mode");
    } else {
      mode = 1;
      Serial.println("Switched to Mode 1: Robot Movement");
    }

    delay(250); // simple debounce
  }

  lastButtonState = buttonState;

  // Read MPU6050
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  int xTilt = accel.acceleration.x * 100;
  int yTilt = accel.acceleration.y * 100;

  int tiltForwardBack = map(yTilt, -900, 900, maxControl, minControl);
  int tiltLeftRight   = map(xTilt, -900, 900, minControl, maxControl);

  tiltForwardBack = constrain(tiltForwardBack, minControl, maxControl);
  tiltLeftRight   = constrain(tiltLeftRight, minControl, maxControl);

  // Apply dead zone
  if (abs(tiltForwardBack) <= deadZone) {
    tiltForwardBack = 0;
  }

  if (abs(tiltLeftRight) <= deadZone) {
    tiltLeftRight = 0;
  }

  // Reset all outputs
  int forwardBack = 0;
  int leftRight = 0;
  int upDown = 0;
  int yaw = 0;

  if (mode == 1) {
    // Robot movement mode
    forwardBack = tiltForwardBack;
    leftRight = tiltLeftRight;
  } else {
    // Car receiver will stop motors in Mode 2
    forwardBack = 0;
    leftRight = 0;
    upDown = 0;
    yaw = 0;
  }

  // Fill data packet
  controlData.mode = mode;
  controlData.forwardBack = forwardBack;
  controlData.leftRight = leftRight;
  controlData.upDown = upDown;
  controlData.yaw = yaw;
  controlData.armed = false; // arm/disarm button will be added later

  // Send data wirelessly
  esp_now_send(broadcastAddress, (uint8_t *) &controlData, sizeof(controlData));

  // Print local glove values
  Serial.print("Mode: ");
  Serial.print(controlData.mode);

  Serial.print(" | FB: ");
  Serial.print(controlData.forwardBack);

  Serial.print(" | LR: ");
  Serial.print(controlData.leftRight);

  Serial.print(" | Armed: ");
  Serial.println(controlData.armed ? "YES" : "NO");

  delay(100);
}
