#include <QMC5883LCompass.h>
#include <Wire.h> // Include Wire for ESP32 I2C

QMC5883LCompass compass;

// --- PINS ---
#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33
#define ENA 14    
#define ENB 32

int slowSpeed = 180;
int fastSpeed = 255;

// State Machine Variable - Now starts at 0 for Auto-Align
int currentStep = 0; 

// --- MOTOR CONTROL FUNCTIONS ---
void carstop() {
  digitalWrite(ENA, LOW); digitalWrite(ENB, LOW);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

void carforwardA() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); 
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  analogWrite(ENA, slowSpeed); analogWrite(ENB, slowSpeed);
}

void carbackwardA() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); 
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, slowSpeed); analogWrite(ENB, slowSpeed);
}

void carleftA() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); 
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, fastSpeed); analogWrite(ENB, fastSpeed);
}

void carrightA() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); 
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, fastSpeed); analogWrite(ENB, fastSpeed);
}

// --- COMPASS HELPER FUNCTIONS ---

// Gets the current 0-359 degree angle
int getAngle() {
  compass.read();
  int a = compass.getAzimuth();
  if (a < 0) {
    a = a + 360;
  }
  Serial.println(a);
  return a;
}

// Keeps turning right until reaching the target angle (+/- 3 degrees)
void turnRightUntil(int targetAngle) {
  carrightA();
  while (true) {
    int currentAngle = getAngle();
    Serial.print("Turning Right... Current: "); Serial.println(currentAngle);
    
    int diff = abs(currentAngle - targetAngle);
    if (diff > 180) diff = 360 - diff; 
    
    if (diff <= 3) { break; }
    delay(50);
  }
  carstop();
}

// Keeps turning left until reaching the target angle (+/- 3 degrees)
void turnLeftUntil(int targetAngle) {
  carleftA();
  while (true) {
    int currentAngle = getAngle();
    Serial.print("Turning Left... Current: "); Serial.println(currentAngle);
    
    int diff = abs(currentAngle - targetAngle);
    if (diff > 180) diff = 360 - diff; 
    
    if (diff <= 3) { break; }
    delay(50);
  }
  carstop();
}

// SMART AUTO-ALIGN FUNCTION
// Figures out the fastest way to spin to the target angle
void autoAlign(int targetAngle) {
  int currentAngle = getAngle();
  int diff = targetAngle - currentAngle;

  // Normalize the difference to find the shortest turning direction
  if (diff > 180) diff -= 360;
  else if (diff < -180) diff += 360;

  // If already within 3 degrees, do nothing
  if (abs(diff) <= 3) {
    return; 
  }

  // Turn right if diff is positive, left if negative
  if (diff > 0) {
    turnRightUntil(targetAngle);
  } else {
    turnLeftUntil(targetAngle);
  }
}

// --- SETUP ---
void setup() {
  Serial.begin(115200);
  Wire.begin(22, 23); 
  
  compass.init();
  compass.setCalibration(4036, 6213, -7885, -5492, -3267, -2691);
  
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT); pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  carstop();
  
  delay(3000); 
  Serial.println("Starting Lawnmower Pattern...");
}

// --- MAIN LOOP (STATE MACHINE) ---
void loop() {
  
  switch (currentStep) {
    
    case 0:
      Serial.println("Step 0: Auto-Aligning to start position (212 degrees)...");
      autoAlign(120); // Car finds its own starting angle
      delay(1000); // Pause for 1 second before moving forward
      currentStep = 1; // Move to Step 1
      break;

    case 1:
      Serial.println("Step 1: Forward at 212 degrees");
      carforwardA();
      delay(2000); 
      carstop();
      delay(500);
      currentStep = 2; 
      break;

    case 2:
      Serial.println("Step 2: Turn right to 272 degrees");
      turnRightUntil(190); 
      delay(500);
      
      Serial.println("Step 2: Lane change forward");
      carforwardA();
      delay(500); 
      carstop();
      delay(500);
      currentStep = 3;
      break;

    case 3:
      Serial.println("Step 3: Turn right to 3 degrees (Setting direction)");
      turnRightUntil(280); 
      delay(500);
      currentStep = 4;
      break;

    case 4:
      Serial.println("Step 4: Forward at 3 degrees");
      carforwardA();
      delay(2000); 
      carstop();
      delay(500);
      currentStep = 5;
      break;

    case 5:
      Serial.println("Step 5: Turn left to 272 degrees");
      turnLeftUntil(200); 
      delay(500);
      
      Serial.println("Step 5: Lane change forward");
      carforwardA();
      delay(500); 
      carstop();
      delay(500);
      currentStep = 6;
      break;

    case 6:
      Serial.println("Step 6: Turn left to 212 degrees (Setting direction)");
      turnLeftUntil(120); 
      delay(500);
      currentStep = 1; // Restart pattern (skip auto-align)
      break;
  }
}
