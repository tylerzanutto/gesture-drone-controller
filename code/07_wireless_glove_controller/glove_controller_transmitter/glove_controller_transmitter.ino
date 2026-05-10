/*
  ESP32 Glove Drone Controller - Wireless Glove Transmitter

  Reads MPU6050 tilt values and a mode button, then sends drone-style
  control values wirelessly to a second ESP32 using ESP-NOW.

  Mode 1: Movement
    Tilt forward/back -> Forward/Back
    Tilt left/right   -> Left/Right

  Mode 2: Height/Yaw
    Tilt forward/back -> Up/Down
    Tilt left/right   -> Yaw
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
int mode = 1; // 1 = movement, 2 = height/yaw
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

// Corrected callback for newer ESP32 board package versions
void onDataSent(const wifi_tx_info_t *info, esp_now_send_status_t status) {
  // Uncomment this line if you want to see send status
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Send Success" : "Send Fail");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(modeButtonPin, INPUT_PULLUP);

  // MPU6050 setup
  Wire.begin(21, 22); // SDA = GPIO 21, SCL = GPIO 22

  Serial.println("Starting wireless glove controller...");

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
  Serial.println("Mode 1: Movement");
}

void loop() {
  // Read mode button
  int buttonState = digitalRead(modeButtonPin);

  // Detect button press, not button hold
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

  // Apply dead zone
  if (abs(tiltForwardBack) <= deadZone) {
    tiltForwardBack = 0;
  }

  if (abs(tiltLeftRight) <= deadZone) {
    tiltLeftRight = 0;
  }

  // Reset all drone-style outputs
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

  Serial.print(" | Forward/Back: ");
  Serial.print(controlData.forwardBack);

  Serial.print(" | Left/Right: ");
  Serial.print(controlData.leftRight);

  Serial.print(" | Up/Down: ");
  Serial.print(controlData.upDown);

  Serial.print(" | Yaw: ");
  Serial.println(controlData.yaw);

  delay(100);
}
