/*
  ESP32 Glove Drone Controller - Receiver Display

  Receives real glove control data from the glove ESP32 using ESP-NOW.
  Prints Mode, Forward/Back, Left/Right, Up/Down, Yaw, and Armed state.
*/

#include <WiFi.h>
#include <esp_now.h>

// Data structure must match the transmitter exactly
typedef struct ControlData {
  int mode;
  int forwardBack;
  int leftRight;
  int upDown;
  int yaw;
  bool armed;
} ControlData;

ControlData receivedData;

void onDataReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  Serial.print("Mode: ");
  Serial.print(receivedData.mode);

  Serial.print(" | Forward/Back: ");
  Serial.print(receivedData.forwardBack);

  Serial.print(" | Left/Right: ");
  Serial.print(receivedData.leftRight);

  Serial.print(" | Up/Down: ");
  Serial.print(receivedData.upDown);

  Serial.print(" | Yaw: ");
  Serial.print(receivedData.yaw);

  Serial.print(" | Armed: ");
  Serial.println(receivedData.armed ? "YES" : "NO");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onDataReceive);

  Serial.println("Receiver ready");
}

void loop() {
  // Data is handled by onDataReceive()
}
