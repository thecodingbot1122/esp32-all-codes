#include <Arduino.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Motor pins
#define IN1 27   // Left Motor
#define IN2 26   // Left Motor
#define IN3 25   // Right Motor
#define IN4 33   // Right Motor

// Task handles
TaskHandle_t BTTaskHandle;
TaskHandle_t MotorTaskHandle;

// Command variable
volatile char command = 'S'; // Start stopped

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT_CAR");

  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopCar(); // start stopped

  // Create FreeRTOS tasks
  xTaskCreatePinnedToCore(readBluetoothTask, "BT Task", 2048, NULL, 1, &BTTaskHandle, 0);
  xTaskCreatePinnedToCore(controlMotorTask, "Motor Task", 2048, NULL, 1, &MotorTaskHandle, 1);

  Serial.println("ESP32 Bluetooth Car Ready with FreeRTOS!");
}

void loop() {
  // Nothing needed here; tasks run independently
}

// ===== FreeRTOS Task 1: Read Bluetooth Commands =====
void readBluetoothTask(void *pvParameters) {
  for (;;) {
    if (SerialBT.available()) {
      char cmd = SerialBT.read();
      command = cmd; // update global command

      Serial.print("Command Received: ");
      Serial.println(cmd);
    }
    vTaskDelay(50 / portTICK_PERIOD_MS); // small delay
  }
}

// ===== FreeRTOS Task 2: Control Motors =====
void controlMotorTask(void *pvParameters) {
  for (;;) {
    switch (command) {
      case 'F': forward(); break;
      case 'B': backward(); break;
      case 'L': left(); break;
      case 'R': right(); break;
      case 'S': stopCar(); break;
    }
    vTaskDelay(50 / portTICK_PERIOD_MS); // control update rate
  }
}

// ===== Motor Functions (basic digitalWrite) =====
void forward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("FORWARD");
}

void backward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("BACKWARD");
}

void left() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("LEFT TURN");
}

void right() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("RIGHT TURN");
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("STOP");
}
