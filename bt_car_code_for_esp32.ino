#include <BluetoothSerial.h>

BluetoothSerial ESP_BT;

#define ENA 13  
#define ENB 25  
#define IN1 12 //rightside forward direction
#define IN2 14 //rightside backward direction
#define IN3 26 //leftside backward direction
#define IN4 27 //leftside forward direction

int motorSpeed = 255;  // Adjust speed (0 - 255)

void setup() {
  Serial.begin(9600);
  ESP_BT.begin("ustad zulu khan");  // Bluetooth device name
  Serial.println("Bluetooth Device is Ready to Pair");

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Set initial motor state
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
  stopMotors();
}

void loop() {
  if (ESP_BT.available()) {
    char command = ESP_BT.read();
    Serial.println(command);

    switch (command) {
      case 'F': moveForward(); break;
      case 'B': moveBackward(); break;
      case 'L': turnLeft(); break;
      case 'R': turnRight(); break;
      case 'S': stopMotors(); break;  // 'S' for stopping the motors
      default: stopMotors(); break;
    }
  }
}

// Function to move forward
void moveForward() {
  Serial.println("Moving Forward");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3,LOW );
  digitalWrite(IN4, HIGH );
}

// Function to move backward
void moveBackward() {
  Serial.println("Moving Backward");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH );
  digitalWrite(IN4,LOW );
}

// Function to turn left
void turnLeft() {
  Serial.println("Turning Left");
  digitalWrite(IN1, HIGH );
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// Function to turn right
void turnRight() {
  Serial.println("Turning Right");
  digitalWrite(IN1,LOW );
  digitalWrite(IN2,HIGH );
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// Function to stop the motors
void stopMotors() {
  Serial.println("Stopping Motors");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
