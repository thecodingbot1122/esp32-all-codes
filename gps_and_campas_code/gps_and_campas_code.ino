#include <TinyGPS++.h>
#include <QMC5883LCompass.h>
#include <HardwareSerial.h>

// --- Configuration ---
// GPS Pins (ESP32 specific)
#define RXPin 16
#define TXPin 17
#define GPSBaud 9600

// --- Objects ---
TinyGPSPlus gps;
HardwareSerial gpsSerial(2); // Use UART2
QMC5883LCompass compass;

// --- Field Scanning Path ---
// Replace these with your calculated zigzag waypoints
struct Point { double lat; double lng; };
Point path[] = {
  {24.8607, 67.0011}, // Point 1 (Start)
  {24.8609, 67.0015}, // Point 2 (End of row 1)
  {24.8610, 67.0015}, // Point 3 (Start of row 2)
  {24.8610, 67.0011}  // Point 4 (End of row 2)
};

int currentWaypoint = 0;
int totalWaypoints = 4;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
  
  // Initialize Compass (0x0D)
  compass.init();
  compass.setCalibration(-1000, 1000, -1000, 1000, -1000, 1000); // RUN CALIBRATION EXAMPLE FIRST!
  
  Serial.println("Robot Ready. Waiting for GPS...");
}

void loop() {
  // 1. Read GPS Data
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // 2. Navigation Logic (Runs only if we have a valid location)
  if (gps.location.isValid() && currentWaypoint < totalWaypoints) {
    
    double currentLat = gps.location.lat();
    double currentLng = gps.location.lng();
    double targetLat = path[currentWaypoint].lat;
    double targetLng = path[currentWaypoint].lng;

    // Calculate distance and required heading
    unsigned long distanceToTarget = gps.distanceBetween(currentLat, currentLng, targetLat, targetLng);
    double targetHeading = gps.courseTo(currentLat, currentLng, targetLat, targetLng);

    // Read Compass (Current Heading)
    compass.read();
    int currentHeading = compass.getAzimuth(); // 0-360 degrees
    
    // 3. Check if reached (within 2 meters)
    if (distanceToTarget < 2.0) {
      
      Serial.println("Waypoint Reached! Moving to next.");
      currentWaypoint++;
      delay(1000); // Pause briefly
      return;
    }

    // 4. Steering Logic
    double error = targetHeading - currentHeading;
    // Adjust for 360 degree wrap-around
    if (error < -180) error += 360;
    if (error > 180) error -= 360;

    Serial.print("Dist: "); Serial.print(distanceToTarget);
    Serial.print("m | Error: "); Serial.println(error);

    if (abs(error) < 10) {
     
    } else if (error > 0) {
      
    } else {
      
    }
  }
}
