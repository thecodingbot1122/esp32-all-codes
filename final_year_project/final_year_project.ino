#include <Servo.h>

Servo scanServo;

// -------- MOTOR PINS --------
#define ENA 2
#define IN1 3
#define IN2 4
#define ENB 7
#define IN3 5
#define IN4 6

// -------- SENSOR --------
#define SENSOR_PIN A0
#define THRESHOLD 800

int sensorValue = 0;
bool metalFound = false;

void setup() {
  // Motor pins
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Sensor
  pinMode(SENSOR_PIN, INPUT);

  // Servo
  scanServo.attach(9);
  scanServo.write(80);

  Serial.begin(9600);
  Serial.println("Metal Detecting Robot Started");
}

// ---------------- MOTOR FUNCTIONS ----------------
void moveForward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3,  LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 130);
  analogWrite(ENB, 130);
}

void stopCar() {
    digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// ---------------- SERVO SCAN ----------------
void fastScan() {
  for (int pos = 0; pos <= 180; pos += 4) {
    scanServo.write(pos);
    delay(15);
  }
  for (int pos = 180; pos >= 0; pos -= 4) {
    scanServo.write(pos);
    delay(15);
  }
}

void slowScanAndDetect() {
  for (int pos = 25; pos <= 150; pos += 1) {
    scanServo.write(pos);
    delay(15);

    sensorValue = analogRead(SENSOR_PIN);
    Serial.print("Slow Scan Value: ");
    Serial.println(sensorValue);

    if (sensorValue > THRESHOLD) {
      while(true)
      {
         sensorValue = analogRead(SENSOR_PIN);
          scanServo.write(pos);
          if (sensorValue < THRESHOLD)
          {
            break;
          }
         delay(15);
      }
      
     
    }
  }
}

// ---------------- MAIN LOOP ----------------
void loop() {

  sensorValue = analogRead(SENSOR_PIN);
  Serial.print("Sensor: ");
  Serial.println(sensorValue);

  if (sensorValue > THRESHOLD) {
      Serial.println("Metal Detected! Stopping...");
      stopCar();
      delay(500);
      slowScanAndDetect();
    }
  else {
    // Metal found → STOP FOREVER
     moveForward();
    fastScan();
  }
}
