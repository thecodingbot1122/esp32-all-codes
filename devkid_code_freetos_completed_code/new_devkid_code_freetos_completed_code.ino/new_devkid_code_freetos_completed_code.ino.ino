/*********
  Project: ESP32 Landmine Robot - Autonomous Navigation & Detection
*********/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <TinyGPS++.h>
#include <QMC5883LCompass.h>

// --- Configuration ---
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
TinyGPSPlus gps;
QMC5883LCompass compass;

#define GPSBaud 9600
#define GPS_RX 16
#define GPS_TX 17

// Servo & Motor Config
#define SERVOMIN 150
#define SERVOMAX 600
#define SERVO_FREQ 50
int servoChannels[7] = {0, 10, 11, 12, 13, 14, 15};
int baseservo = 0;

// --- Motor Pin Definitions ---
#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33
#define ENA 14  
#define ENB 32
const int analogPin = 13;

// Navigation Data
struct Waypoint { double lat; double lng; bool isSet; };
Waypoint path[4] = {{0,0,false}, {0,0,false}, {0,0,false}, {0,0,false}};
int currentWaypoint = 0;

// Speed Settings
int scanStepSize = 4;
int scanTickDelay = 10;
int motorSpeed = 130; 

// State
HardwareSerial mySerial(2); // UART for S3 Communication
HardwareSerial gpsSerial(1); // UART for GPS Module
String message = "";
bool autoModeActive = false;
bool manualScanActive = false;
bool metalDetectedLock = false;

TaskHandle_t ManualTask, ScanTask, AutoTask;

// --- Helper Functions ---
int angleToPulse(int angle) { return map(angle, 0, 180, SERVOMIN, SERVOMAX); }

void carstop() {
  digitalWrite(ENA, LOW); digitalWrite(ENB, LOW);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// ==========================================
// TASK 1: MANUAL & COMMUNICATION (Core 0)
// ==========================================
void ManualTaskCode(void * pvParameters) {
  for(;;) {
    if (mySerial.available()) {
      message = mySerial.readStringUntil('\n');
      message.trim();
      
      if (!metalDetectedLock && !autoModeActive) {
        // FORWARD
        if (message == "F") { 
          digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); 
          digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); 
          analogWrite(ENA, motorSpeed); analogWrite(ENB, motorSpeed); 
        }
        // BACKWARD
        else if (message == "B") { 
          digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); 
          digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); 
          analogWrite(ENA, motorSpeed); analogWrite(ENB, motorSpeed); 
        }
        // LEFT TURN (Left Back, Right Forward)
        else if (message == "L") { 
          digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); 
          digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); 
          analogWrite(ENA, motorSpeed); analogWrite(ENB, motorSpeed); 
        }
        // RIGHT TURN (Left Forward, Right Back)
        else if (message == "R") { 
          digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); 
          digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); 
          analogWrite(ENA, motorSpeed); analogWrite(ENB, motorSpeed); 
        }
        // STOP
        else if (message == "S") {
          carstop();
        }
      }
      if (message == "SCAN") manualScanActive = !manualScanActive;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// ==========================================
// TASK 2: SCANNING & DETECTION (Core 1)
// ==========================================
void ScanTaskCode(void * pvParameters) {
  int angle = 0;
  bool directionUp = true;
  for(;;) {
    if (autoModeActive || manualScanActive) {
      pwm.setPWM(baseservo, 0, angleToPulse(angle));
      
      int sensorVal = analogRead(analogPin);
      if (sensorVal > 3500) {
        metalDetectedLock = true; 
        carstop();
        autoModeActive = false;
        manualScanActive = false;
        Serial.println("!!! MINE DETECTED !!!");
        pwm.setPWM(baseservo, 0, angleToPulse(angle <= 90 ? 130 : 0));
        vTaskDelay(5000 / portTICK_PERIOD_MS); 
        metalDetectedLock = false; 
      }

      if (directionUp) { angle += scanStepSize; if (angle >= 130) { angle = 130; directionUp = false; } }
      else { angle -= scanStepSize; if (angle <= 0) { angle = 0; directionUp = true; } }
      vTaskDelay(scanTickDelay / portTICK_PERIOD_MS);
    } else {
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
  }
}

// ==========================================
// TASK 3: AUTOMATIC NAVIGATION (Core 1)
// ==========================================
void AutoTaskCode(void * pvParameters) {
  for(;;) {
    // 1. Process GPS incoming data
    while (gpsSerial.available() > 0) { gps.encode(gpsSerial.read()); }

    // 2. Handle Coordinate Input from S3
    if (message.startsWith("G") && message.indexOf(':') != -1) {
      int id = message.substring(1, message.indexOf(':')).toInt();
      int comma = message.indexOf(',');
      if (id >= 1 && id <= 4) {
        path[id-1].lat = message.substring(message.indexOf(':')+1, comma).toDouble();
        path[id-1].lng = message.substring(comma+1).toDouble();
        path[id-1].isSet = true;
        Serial.printf("Stored G%d\n", id);
      }
      message = "";
    }

    // 3. Execution Logic
    if (message == "Y") { autoModeActive = true; currentWaypoint = 0; message = ""; }
    if (message == "Z") { autoModeActive = false; carstop(); message = ""; }

    if (autoModeActive && !metalDetectedLock) {
      if (gps.location.isValid() && currentWaypoint < 4) {
        double d = gps.distanceBetween(gps.location.lat(), gps.location.lng(), path[currentWaypoint].lat, path[currentWaypoint].lng);
        double targetH = gps.courseTo(gps.location.lat(), gps.location.lng(), path[currentWaypoint].lat, path[currentWaypoint].lng);
        
        compass.read();
        int currentH = compass.getAzimuth();
        
        if (d < 2.0) {
          carstop();
          currentWaypoint++;
          vTaskDelay(1000 / portTICK_PERIOD_MS);
        } else {
          double error = targetH - currentH;
          if (error < -180) error += 360;
          if (error > 180) error -= 360;

          if (abs(error) < 15) { // Drive Forward
            digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
          } else if (error > 0) { // Turn Right
            digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
          } else { // Turn Left
            digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
          }
          analogWrite(ENA, motorSpeed); analogWrite(ENB, motorSpeed);
        }
      }
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, 21, 19);  // UART to S3
  gpsSerial.begin(GPSBaud, SERIAL_8N1, GPS_RX, GPS_TX); // UART to GPS
  
  Wire.begin(22, 23);
  Wire.setClock(400000);
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);
  compass.init();
  // compass.setCalibration(...); // ADD YOUR CALIBRATION VALUES HERE

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT); pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  
  xTaskCreatePinnedToCore(ManualTaskCode, "Manual", 10000, NULL, 2, &ManualTask, 0);
  xTaskCreatePinnedToCore(ScanTaskCode, "Scan", 10000, NULL, 1, &ScanTask, 1);
  xTaskCreatePinnedToCore(AutoTaskCode, "Auto", 10000, NULL, 1, &AutoTask, 1);
}

void loop() {}
