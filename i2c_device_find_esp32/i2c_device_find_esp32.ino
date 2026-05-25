#include <Wire.h>

void setup() {
  Serial.begin(115200);

  Wire.begin(18, 19);  // SDA = 18, SCL = 19 (change here)
}

void loop() {
  for (int addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    
    if (Wire.endTransmission() == 0) {
      Serial.print("Found: 0x");
      Serial.println(addr, HEX);
    }
  }

  delay(3000);
}
