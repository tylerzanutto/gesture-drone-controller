const int modeButtonPin = 18;

int mode = 1;
int lastButtonState = HIGH;

void setup() {
  Serial.begin(115200);
  pinMode(modeButtonPin, INPUT_PULLUP);

  Serial.println("Mode button test started");
  Serial.println("Current mode: 1");
}

void loop() {
  int buttonState = digitalRead(modeButtonPin);

  // Detect button press, not button hold
  if (lastButtonState == HIGH && buttonState == LOW) {
    if (mode == 1) {
      mode = 2;
    } else {
      mode = 1;
    }

    Serial.print("Mode changed to: ");
    Serial.println(mode);

    delay(250); // simple debounce
  }

  lastButtonState = buttonState;
}
