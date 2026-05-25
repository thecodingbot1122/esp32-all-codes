#include <TinyGPS++.h>

TinyGPSPlus gps;
#define RXD2 16
#define TXD2 17

void setup() {
  Serial.begin(115200);
  // Initialize Serial2 for GPS at 9600 baud
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); 
}

void loop() {
  
  while (Serial2.available() > 0) {
    if (gps.encode(Serial2.read())) {
      if (gps.location.isValid()) {
        Serial.print("Lat: "); Serial.println(gps.location.lat(), 6);
        Serial.print("Lng: "); Serial.println(gps.location.lng(), 6);
      }
    }
  }
}
