#include <Wire.h>
#include <QMC5883LCompass.h>

QMC5883LCompass compass;

// --- Motor Pins ---
#define ENA 14  
#define IN1 27
#define IN2 26
#define ENB 32
#define IN3 25
#define IN4 33

// --- Navigation States ---
// Added FIND_NORTH as the first step
enum DriveState { FIND_NORTH, FORWARD_ROW, TURN_90_DEG, SHIFT_LANE, RETURN_TURN };
DriveState currentState = FIND_NORTH;

int baseSpeed = 180;
int targetHeading = 0; // 0 is North
unsigned long timer = 0;
bool turnRight = true; 

void setup() {
  Serial.begin(115200);
  Wire.begin(22, 23);
  compass.init();
  
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  Serial.println("Robot Initialized. Finding North...");
  timer = millis();
}

void loop() {
  compass.read();
  int currentHeading = compass.getAzimuth();

  switch (currentState) {
    
    case FIND_NORTH:
      // 1. SPIN UNTIL FACING NORTH (0 Degrees)
      if (getAngleDiff(0, currentHeading) > 5) {
        spinRight(); // Keep spinning until North is found
      } else {
        stopCar();
        Serial.println("North Found! Starting Row 1...");
        targetHeading = 0; // Set target to North
        currentState = FORWARD_ROW;
        timer = millis();
      }
      break;

    case FORWARD_ROW:
      // 2. DRIVE NORTH (or South on return)
      autoSteer(targetHeading, currentHeading);
      if (millis() - timer > 5000) {
        stopCar();
        // Calculate 90 degree turn from current target
        if(turnRight) targetHeading = (targetHeading + 90) % 360;
        else targetHeading = (targetHeading - 90 + 360) % 360;
        currentState = TURN_90_DEG;
        timer = millis();
      }
      break;

    case TURN_90_DEG:
      // 3. TURN EAST OR WEST
      if (getAngleDiff(targetHeading, currentHeading) > 5) {
        if(turnRight) spinRight(); else spinLeft();
      } else {
        stopCar();
        currentState = SHIFT_LANE;
        timer = millis();
      }
      break;

    case SHIFT_LANE:
      // 4. MOVE TO NEXT ROW
      moveForward();
      if (millis() - timer > 2000) {
        stopCar();
        // Flip heading 180 degrees (Facing South for the return)
        targetHeading = (targetHeading + (turnRight ? 90 : -90) + 360) % 360; 
        currentState = RETURN_TURN;
        timer = millis();
      }
      break;

    case RETURN_TURN:
      // 5. TURN TO FACE SOUTH (180)
      if (getAngleDiff(targetHeading, currentHeading) > 5) {
        if(turnRight) spinRight(); else spinLeft();
      } else {
        stopCar();
        turnRight = !turnRight; // Flip turn for next row
        currentState = FORWARD_ROW;
        timer = millis();
      }
      break;
  }
}

// --- Navigation Helpers ---

void autoSteer(int target, int current) {
  int drift = target - current;
  if (drift > 180) drift -= 360;
  if (drift < -180) drift += 360;

  if (drift > 5) moveMotors(baseSpeed - 40, baseSpeed + 40); 
  else if (drift < -5) moveMotors(baseSpeed + 40, baseSpeed - 40); 
  else moveMotors(baseSpeed, baseSpeed);
}

int getAngleDiff(int target, int current) {
  int diff = abs(target - current);
  if (diff > 180) diff = 360 - diff;
  return diff;
}

// --- Motor Controls ---
void moveMotors(int sL, int sR) {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  analogWrite(ENA, sL); analogWrite(ENB, sR);
}

void moveForward() { moveMotors(baseSpeed, baseSpeed); }

void spinRight() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, HIGH); analogWrite(ENB, HIGH);
}

void spinLeft() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, HIGH); analogWrite(ENB, HIGH);
}

void stopCar() { analogWrite(ENA, 0); analogWrite(ENB, 0); }
