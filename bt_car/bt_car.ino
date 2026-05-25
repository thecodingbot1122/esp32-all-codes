#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Motor pins
#define IN1 27   // Left Motor
#define IN2 26   // Left Motor
#define IN3 25   // Right Motor
#define IN4 33   // Right Motor

// Enable pins
#define ENA 14   // Left Motor Enable
#define ENB 32   // Right Motor Enable

char command;

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32_BT_CAR");

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Enable motors (full speed)
  digitalWrite(ENA, 200);
  digitalWrite(ENB, 200);

  stopCar();
  Serial.println("ESP32 Bluetooth Car Ready (EN Pins Enabled)");
}

void loop() {
  if (SerialBT.available()) {
    command = SerialBT.read();

    Serial.print("Command Received: ");
    Serial.println(command);

    switch (command) {
      case 'F':
        forward();
        break;
      case 'B':
        backward();
        break;
      case 'L':
        left();
        break;
      case 'R':
        right();
        break;
      case 'S':
        stopCar();
        break;
    }
  }
}

// ===== Motor Control (INVERTED LOGIC) =====

void forward() {
  Serial.println("FORWARD");
  Serial.println("Left Motor  : Forward");
  Serial.println("Right Motor : Forward");

  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void backward() {
  Serial.println("BACKWARD");
  Serial.println("Left Motor  : Backward");
  Serial.println("Right Motor : Backward");

  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void left() {
  Serial.println("LEFT TURN");
  Serial.println("Left Motor  : Backward");
  Serial.println("Right Motor : Forward");

  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void right() {
  Serial.println("RIGHT TURN");
  Serial.println("Left Motor  : Forward");
  Serial.println("Right Motor : Backward");

  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void stopCar() {
  Serial.println("STOP");
  Serial.println("Left Motor  : Stop");
  Serial.println("Right Motor : Stop");

  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
