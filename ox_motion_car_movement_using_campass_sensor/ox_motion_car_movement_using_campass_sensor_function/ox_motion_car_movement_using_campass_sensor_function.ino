#include <Wire.h>
#include <QMC5883LCompass.h>
#include <BluetoothSerial.h>

QMC5883LCompass compass;
BluetoothSerial SerialBT;

// ---------------- MOTOR PINS (ESP32) ----------------
#define IN1 27 
#define IN2 26 
#define IN3 25 
#define IN4 33 
#define ENA 14 
#define ENB 32 

// ---------------- AUTOPILOT SETTINGS ----------------
const int sweepTime = 8000;     
const int stepTime  = 1500;     
const int autoBaseSpeed = 200;      
const int autoTurnSpeed = 130;      

// ---------------- MANUAL DRIVING SPEED ----------------
int manualSpeed = 255; // Full speed for manual driving

// ---------------- STATE MACHINES & FLAGS ----------------
enum State { SWEEP_1, TURN_R1, STEP_1, TURN_R2, SWEEP_2, TURN_L1, STEP_2, TURN_L2 };
State autoState = SWEEP_1;

bool isAutopilotActive = false;
unsigned long startTime;
float targetAngle = 0;       
float currentHeadingLock = 0;

// ---------------- MANUAL DRIVING CONTROLS ----------------
void stopMotors() {
  analogWrite(ENA, 0); analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void driveForward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, manualSpeed); analogWrite(ENB, manualSpeed);
}

void driveBackward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  analogWrite(ENA, manualSpeed); analogWrite(ENB, manualSpeed);
}

void turnRightManual() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, manualSpeed); analogWrite(ENB, manualSpeed);
}

void turnLeftManual() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, manualSpeed); analogWrite(ENB, manualSpeed);
}

// ---------------- COMPASS FUNCTIONS (For Autopilot) ----------------
float getHeading() {
  compass.read();
  float angle = compass.getAzimuth();
  if (angle < 0) angle += 360;
  return angle;
}

bool reachedTarget(float target) {
  float current = getHeading();
  float diff = abs(current - target);
  if (diff > 180.0) diff = 360.0 - diff;
  return (diff <= 5.0); 
}

void moveStraightWithCompass(float targetH) {
  float currentH = getHeading();
  float error = currentH - targetH;
  if (error < -180) error += 360;
  if (error > 180)  error -= 360;

  float Kp = 3.0; 
  int correction = error * Kp;
  int leftSpeed = autoBaseSpeed - correction;
  int rightSpeed = autoBaseSpeed + correction;

  leftSpeed = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);

  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, leftSpeed); analogWrite(ENB, rightSpeed);
}

void autoTurnRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, autoTurnSpeed); analogWrite(ENB, autoTurnSpeed);
}

void autoTurnLeft() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, autoTurnSpeed); analogWrite(ENB, autoTurnSpeed);
}

// ---------------- AUTOPILOT LOGIC ----------------
void runAutopilot() {
  switch (autoState) {
    case SWEEP_1:
      moveStraightWithCompass(currentHeadingLock);
      if (millis() - startTime >= sweepTime) {
        stopMotors(); delay(300);
        targetAngle = currentHeadingLock + 90;
        if (targetAngle >= 360) targetAngle -= 360;
        startTime = millis(); autoState = TURN_R1;
      }
      break;
    case TURN_R1:
      autoTurnRight();
      if (reachedTarget(targetAngle)) {
        stopMotors(); delay(300);
        currentHeadingLock = targetAngle; 
        startTime = millis(); autoState = STEP_1;
      }
      break;
    case STEP_1:
      moveStraightWithCompass(currentHeadingLock);
      if (millis() - startTime >= stepTime) {
        stopMotors(); delay(300);
        targetAngle = currentHeadingLock + 90;
        if (targetAngle >= 360) targetAngle -= 360;
        startTime = millis(); autoState = TURN_R2;
      }
      break;
    case TURN_R2:
      autoTurnRight();
      if (reachedTarget(targetAngle)) {
        stopMotors(); delay(300);
        currentHeadingLock = targetAngle;
        startTime = millis(); autoState = SWEEP_2;
      }
      break;
    case SWEEP_2:
      moveStraightWithCompass(currentHeadingLock);
      if (millis() - startTime >= sweepTime) {
        stopMotors(); delay(300);
        targetAngle = currentHeadingLock - 90;
        if (targetAngle < 0) targetAngle += 360;
        startTime = millis(); autoState = TURN_L1;
      }
      break;
    case TURN_L1:
      autoTurnLeft();
      if (reachedTarget(targetAngle)) {
        stopMotors(); delay(300);
        currentHeadingLock = targetAngle;
        startTime = millis(); autoState = STEP_2;
      }
      break;
    case STEP_2:
      moveStraightWithCompass(currentHeadingLock);
      if (millis() - startTime >= stepTime) {
        stopMotors(); delay(300);
        targetAngle = currentHeadingLock - 90;
        if (targetAngle < 0) targetAngle += 360;
        startTime = millis(); autoState = TURN_L2;
      }
      break;
    case TURN_L2:
      autoTurnLeft();
      if (reachedTarget(targetAngle)) {
        stopMotors(); delay(300);
        currentHeadingLock = targetAngle;
        startTime = millis(); autoState = SWEEP_1; 
      }
      break;
  }
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);
  SerialBT.begin("RoboPlow_ESP32"); // This is the Bluetooth name you will see on your phone!
  
  Wire.begin(22, 23); 
  compass.init();

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);

  delay(1000); 
}

// ---------------- MAIN LOOP ----------------
void loop() {
  // Check if your phone sent a command
  if (SerialBT.available()) {
    char command = SerialBT.read();
    
    // If we receive a manual command, turn off autopilot immediately
    if (command != 'X') {
      isAutopilotActive = false;
    }

    // Execute the command
    switch (command) {
      case 'F': driveForward(); break;    // Forward
      case 'B': driveBackward(); break;   // Backward
      case 'L': turnLeftManual(); break;  // Left
      case 'R': turnRightManual(); break; // Right
      case 'S': stopMotors(); break;      // Stop
      
      // The Magic "X" Button triggers Autopilot
      case 'X': 
        if (!isAutopilotActive) {
          isAutopilotActive = true;
          autoState = SWEEP_1;              // Reset pattern to the start
          currentHeadingLock = getHeading();// Lock onto whatever direction we are currently facing
          startTime = millis();             // Start the timer
          SerialBT.println("Autopilot ENGAGED!");
        }
        break;
    }
  }

  // If Autopilot is active, run the field plow sequence
  if (isAutopilotActive) {
    runAutopilot();
  }
}
