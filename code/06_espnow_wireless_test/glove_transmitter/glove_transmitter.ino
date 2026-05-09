#include <WiFi.h>
#include <esp_now.h>

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

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
  Serial.print("Send status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

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
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Transmitter ready");
}

void loop() {
  controlData.mode = 1;
  controlData.forwardBack = 50;
  controlData.leftRight = -25;
  controlData.upDown = 0;
  controlData.yaw = 0;
  controlData.armed = false;

  esp_err_t result = esp_now_send(
    broadcastAddress,
    (uint8_t *) &controlData,
    sizeof(controlData)
  );

  if (result == ESP_OK) {
    Serial.println("Data sent");
  } else {
    Serial.println("Send error");
  }

  delay(500);
}
