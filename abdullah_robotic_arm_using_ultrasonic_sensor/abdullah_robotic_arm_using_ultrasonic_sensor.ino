#include <Arduino.h>
#include <Servo.h>

// Servos
Servo Baseservo;
Servo Jarservo;
Servo Rightservo;
Servo Leftservo;

// Ultrasonic Sensor Pins
const int trigPin = 11;
const int echoPin = 12;
const int thresholdDistance = 10; // 10 cm par object detect hoga

void setup() {
  Baseservo.attach(7);
  Jarservo.attach(8);
  Rightservo.attach(9);
  Leftservo.attach(10);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  Serial.println("System Ready! Waiting for object...");

  // --- SAFE HOME POSITIONS ---
  Baseservo.write(0);   
  Jarservo.write(130);  
  Rightservo.write(5);  
  Leftservo.write(60);  // Left servo is angle par hold karegi
  delay(1000);
}

void loop() {
  long duration;
  int distance;

  // Sensor Triggering
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // Jab object qareeb aaye
  if (distance > 0 && distance <= thresholdDistance) {
    Serial.println("Object Detected! Sequence Started...");
    
    // Pick aur Drop wala sequence call karna
    executePickAndDrop();
    
    delay(3000); 
    Serial.println("Waiting for new object...");
  }
  delay(100);
}

// ----------------------------------------------------
// MAIN SEQUENCE
// ----------------------------------------------------
void executePickAndDrop() {
  
  jarservomotoropen(); 
  delay(500);

  baseservomotor1();   
  delay(500);

  // Arm neechay karna pick karne ke liye
  // Leftsidemotor1();    // <--- LEFT SERVO COMMENT OUT
  Rightsidemotor1();
  delay(500);

  jarservomotoclose(); 
  delay(500);

  // Arm wapas upar uthana
  Rightsidemotorback1();
  // leftsidemotorback1(); // <--- LEFT SERVO COMMENT OUT
  delay(500);

  baseseroback1();     
  delay(500);

  // Arm neechay karna drop karne ke liye
  // Leftsidemotor2();    // <--- LEFT SERVO COMMENT OUT
  Rightsidemotor2();
  delay(500);

  jarservomotoropen(); 
  delay(500);

  // Khali arm wapas upar uthana
  Rightsidemotorback2();
  // leftsidemotorback2(); // <--- LEFT SERVO COMMENT OUT
  delay(500);

  jarservomotoclose(); 
  delay(500);

  baseservomotorhome(); 
  delay(500);
}

// ----------------------------------------------------
// MOVEMENT FUNCTIONS 
// ----------------------------------------------------

// Jar Functions 
void jarservomotoropen() {
  for(int jar = 130; jar >= 70; jar = jar - 1) {
    Jarservo.write(jar);
    delay(15);
  }
}

void jarservomotoclose() {
  for(int jar = 70; jar <= 130; jar = jar + 1) {
    Jarservo.write(jar);
    delay(15);
  }
}

// Base Functions 
void baseservomotor1() { 
  Baseservo.write(0);
  delay(15);
}

void baseseroback1() { 
  for(int base = 0; base <= 180; base = base + 1) {
    Baseservo.write(base);
    delay(15);
  }
}

void baseservomotorhome() { 
  for(int base = 180; base >= 0; base = base - 1) {
    Baseservo.write(base);
    delay(15);
  }
}

// Right Side Functions 
void Rightsidemotor1() { 
  for(int right = 5; right <= 100; right = right + 1) {
    Rightservo.write(right);
    delay(15);
  }
}

void Rightsidemotorback1() {
  for(int right = 100; right >= 5; right = right - 1) {
    Rightservo.write(right);
    delay(15);
  }
}

// Left Side Functions (Yeh abhi sequence mein call nahi ho rahay)
void Leftsidemotor1() {
  for(int left = 60; left <= 150; left = left + 1) {
    Leftservo.write(left);
    delay(15);
  }
}

void leftsidemotorback1() {
  for(int left = 150; left >= 60; left = left - 1) {
    Leftservo.write(left);
    delay(15);
  }
}

// Right Side Functions (Drop)
void Rightsidemotor2() {
  for(int right = 5; right <= 100; right = right + 1) {
    Rightservo.write(right);
    delay(15);
  }
}

void Rightsidemotorback2() {
  for(int right = 100; right >= 5; right = right - 1) {
    Rightservo.write(right);
    delay(15);
  }
}

// Left Side Functions (Drop)
void Leftsidemotor2() {
  for(int left = 60; left <= 150; left = left + 1) {
    Leftservo.write(left);
    delay(15);
  }
}

void leftsidemotorback2() {
  for(int left = 150; left >= 60; left = left - 1) {
    Leftservo.write(left);
    delay(15);
  }
}
