#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// ==========================================
// 1. HARDWARE CONFIGURATION
// ==========================================
const int EMG_PIN = A0;

// Initialize the PCA9685 at the default I2C address (0x40)
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Servo pulse widths calculated for MG996R
// 275 = ~50 degrees 
// 425 = ~110 degrees 
#define SERVO_MIN 275 
#define SERVO_MAX 425 

// ==========================================
// 2. SIGNAL PROCESSING VARIABLES
// ==========================================
const unsigned long SAMPLE_INTERVAL_MS = 5; // 200Hz sampling rate
unsigned long previousMillis = 0;

// A separate timer just for the Serial Monitor
const unsigned long PRINT_INTERVAL_MS = 50; 
unsigned long previousPrintMillis = 0;

const int WINDOW_SIZE = 50;     // 250ms smoothing window
int readings[WINDOW_SIZE];      // Circular buffer
int readIndex = 0;              
float dynamicBaseline = 0;      // Auto-zeroing tracker

const int MAX_EMG_FLEX = 500; 

// ==========================================
// 3. SETUP ROUTINE
// ==========================================
void setup() {
  Serial.begin(115200); 
  pinMode(EMG_PIN, INPUT);

  Serial.println("Starting Servo Controller...");
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);  

  for (int i = 0; i < WINDOW_SIZE; i++) {
    readings[i] = 0;
  }

  Serial.println("Calibrating... Please keep your muscle completely relaxed.");
  long sum = 0;
  for (int i = 0; i < 200; i++) {
    sum += analogRead(EMG_PIN);
    delay(5); 
  }
  
  dynamicBaseline = sum / 200.0;
  Serial.print("Initial Baseline: ");
  Serial.println(dynamicBaseline);
  delay(1000); 
}

// ==========================================
// 4. MAIN CONTROL LOOP
// ==========================================
void loop() {
  unsigned long currentMillis = millis();

  // Non-blocking timer 1: Run every 5ms for fast sensor reading
  if (currentMillis - previousMillis >= SAMPLE_INTERVAL_MS) {
    previousMillis = currentMillis;

    // --- STEP A: READ & FILTER EMG ---
    int rawValue = analogRead(EMG_PIN);

    if (abs(rawValue - dynamicBaseline) < 50) {
        dynamicBaseline = (0.001 * rawValue) + (0.999 * dynamicBaseline);
    }

    int centeredValue = rawValue - (int)dynamicBaseline;
    readings[readIndex] = centeredValue;
    readIndex = (readIndex + 1) % WINDOW_SIZE;

    unsigned long sumOfSquares = 0;
    for (int i = 0; i < WINDOW_SIZE; i++) {
        long val = readings[i]; 
        sumOfSquares += (val * val);
    }
    int rmsEMG = sqrt(sumOfSquares / WINDOW_SIZE);

    if (rmsEMG < 15) {
        rmsEMG = 0;
    }

    // --- STEP B: CONTROL SERVOS ---
    int pulseLength = map(rmsEMG, 0, MAX_EMG_FLEX, SERVO_MIN, SERVO_MAX);
    pulseLength = constrain(pulseLength, SERVO_MIN, SERVO_MAX);

    for (int servoNum = 0; servoNum < 5; servoNum++) {
      pwm.setPWM(servoNum, 0, pulseLength);
    }

    // --- STEP C: SERIAL MONITORING ---
    // Non-blocking timer 2: Only print to the screen every 50ms
    if (currentMillis - previousPrintMillis >= PRINT_INTERVAL_MS) {
        previousPrintMillis = currentMillis;
        
        Serial.print("RMS_Muscle:");
        Serial.print(rmsEMG);
        Serial.print(",");
        Serial.print("Servo_Position:");
        Serial.println(pulseLength);
    }
  }
}
