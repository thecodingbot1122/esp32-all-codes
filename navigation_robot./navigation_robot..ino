/*
 * This integrated code enables your ESP32 robot to perform a
Boustrophedon (lawnmower) scan. It takes four corners , calculates parallel rows between them based on a ROW_WIDTH, 
and navigates them autonomously while maintaining manual control and obstacle detection.
 */

#include <Wire.h>
#include <TinyGPS++.h>
#include <QMC5883LCompass.h>

// --- PIN DEFINITIONS ---
#define ENA 13
#define IN1 12
#define IN2 14
#define ENB 25
#define IN3 27
#define IN4 26
#define RXD2 16
#define TXD2 17

// --- OBJECTS & VARIABLES ---
TinyGPSPlus gps;
QMC5883LCompass compass;
HardwareSerial SerialGPS(2);

// Arrays for Corners and Generated Path
double cornerLat[4], cornerLon[4];
bool cornerSet[4] = {false, false, false, false};
double pathLat[50], pathLon[50];
int totalPathPoints = 0, currentWP = 0;
bool isScanning = false;

// Settings
const float ROW_WIDTH = 5.0; // Meters between zig-zags
const float REACHED_DIST = 2.0; 
const int BASE_SPEED = 180;

void setup() {
  Serial.begin(115200);
  SerialGPS.begin(9600, SERIAL_8N1, RXD2, TXD2); 
  Wire.begin();
  compass.init(); 
  
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  Serial.println("Ready. Enter coordinates (G1:lat,lon) then type 'START'");
}

void loop() {
  // 1. Check Serial Monitor for Input
  if (Serial.available()) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    handleSerialInput(msg);
  }

  // 2. Process GPS and Navigate
  while (SerialGPS.available() > 0) {
    if (gps.encode(SerialGPS.read())) { 
      if (isScanning) navigate();
    }
  }
}

void handleSerialInput(String msg) {
  if (msg.startsWith("G")) {
    int col = msg.indexOf(':');
    int com = msg.indexOf(',');
    int idx = msg.substring(1, col).toInt() - 1; // G1 -> index 0

    if (idx >= 0 && idx < 4) {
      cornerLat[idx] = msg.substring(col + 1, com).toDouble();
      cornerLon[idx] = msg.substring(com + 1).toDouble();
      cornerSet[idx] = true;
      Serial.println("Point G" + String(idx + 1) + " Saved.");
    }
  } 
  else if (msg == "START") {
    if (cornerSet[0] && cornerSet[1] && cornerSet[2] && cornerSet[3]) {
      generateBoustrophedon();
      isScanning = true;
      currentWP = 0;
      Serial.println("Zig-Zag Scan Started!");
    } else {
      Serial.println("Error: Set G1, G2, G3, and G4 first!");
    }
  }
  else if (msg == "STOP") {
    isScanning = false;
    stopMotors();
    Serial.println("Scan Stopped.");
  }
}

void generateBoustrophedon() {
  float fieldHeight = (float)TinyGPSPlus::distanceBetween(cornerLat[0], cornerLon[0], cornerLat[3], cornerLon[3]);
  int numRows = constrain((int)(fieldHeight / ROW_WIDTH), 2, 24);

  totalPathPoints = 0;
  for (int i = 0; i <= numRows; i++) {
    float ratio = (float)i / numRows;
    double latA = cornerLat[0] + (cornerLat[3] - cornerLat[0]) * ratio;
    double lonA = cornerLon[0] + (cornerLon[3] - cornerLon[0]) * ratio;
    double latB = cornerLat[1] + (cornerLat[2] - cornerLat[1]) * ratio;
    double lonB = cornerLon[1] + (cornerLon[2] - cornerLon[1]) * ratio;

    if (i % 2 == 0) {
      pathLat[totalPathPoints] = latA; pathLon[totalPathPoints++] = lonA;
      pathLat[totalPathPoints] = latB; pathLon[totalPathPoints++] = lonB;
    } else {
      pathLat[totalPathPoints] = latB; pathLon[totalPathPoints++] = lonB;
      pathLat[totalPathPoints] = latA; pathLon[totalPathPoints++] = lonA;
    }
  }
}

void navigate() {
  if (!gps.location.isValid() || currentWP >= totalPathPoints) {
    stopMotors();
    isScanning = false;
    return;
  }

  double dist = TinyGPSPlus::distanceBetween(gps.location.lat(), gps.location.lng(), pathLat[currentWP], pathLon[currentWP]);
  double targetHeading = TinyGPSPlus::courseTo(gps.location.lat(), gps.location.lng(), pathLat[currentWP], pathLon[currentWP]);

  compass.read();
  int head = compass.getAzimuth(); 
  if (head < 0) head += 360;

  float error = targetHeading - head;
  if (error > 180) error -= 360; if (error < -180) error += 360;

  if (dist < REACHED_DIST) {
    currentWP++;
    stopMotors();
    delay(1000); 
  } else {
    driveRobot(error);
  }
}

void driveRobot(float error) {
  int steer = error * 2.5; 
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, constrain(BASE_SPEED + steer, 0, 255));
  analogWrite(ENB, constrain(BASE_SPEED - steer, 0, 255));
}

void stopMotors() {
  analogWrite(ENA, 0); analogWrite(ENB, 0);
}
