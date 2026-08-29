
// TB6612FNG pins connected to ESP32
const int AIN1 = 26;
const int AIN2 = 27;
const int PWMA = 25;

const int BIN1 = 14;
const int BIN2 = 12;
const int PWMB = 33;

const int STBY = 32; // If STBY is connected to 3V3, this pin does not matter

// PWM settings
const int pwmFreq = 1000;
const int pwmResolution = 8; // 0-255 speed range

// Motor speed for testing
const int testSpeed = 120; // 0-255. Start gentle.

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH); // enable motor driver if STBY is connected to GPIO 32

  // New ESP32 PWM setup
  ledcAttach(PWMA, pwmFreq, pwmResolution);
  ledcAttach(PWMB, pwmFreq, pwmResolution);

  stopMotors();

  Serial.println("TB6612FNG motor test starting...");
}

void loop() {
  Serial.println("Left motors forward");
  leftMotor(testSpeed);
  rightMotor(0);
  delay(2000);

  stopMotors();
  delay(1000);

  Serial.println("Left motors backward");
  leftMotor(-testSpeed);
  rightMotor(0);
  delay(2000);

  stopMotors();
  delay(1000);

  Serial.println("Right motors forward");
  leftMotor(0);
  rightMotor(testSpeed);
  delay(2000);

  stopMotors();
  delay(1000);

  Serial.println("Right motors backward");
  leftMotor(0);
  rightMotor(-testSpeed);
  delay(2000);

  stopMotors();
  delay(1000);

  Serial.println("Both sides forward");
  leftMotor(testSpeed);
  rightMotor(testSpeed);
  delay(2000);

  stopMotors();
  delay(1000);

  Serial.println("Both sides backward");
  leftMotor(-testSpeed);
  rightMotor(-testSpeed);
  delay(2000);

  stopMotors();
  delay(3000);
}

// speed: -255 to +255
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
