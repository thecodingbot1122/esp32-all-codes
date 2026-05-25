#include <Wire.h>

#define HMC5883L_ADDR 0x1E

int16_t x, y, z;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  // Configure HMC5883L
  Wire.beginTransmission(HMC5883L_ADDR);
  Wire.write(0x00); // Configuration Register A
  Wire.write(0x70);
  Wire.endTransmission();

  Wire.beginTransmission(HMC5883L_ADDR);
  Wire.write(0x01); // Configuration Register B
  Wire.write(0xA0);
  Wire.endTransmission();

  Wire.beginTransmission(HMC5883L_ADDR);
  Wire.write(0x02); // Mode Register
  Wire.write(0x00); // Continuous mode
  Wire.endTransmission();
}

void loop() {
  Wire.beginTransmission(HMC5883L_ADDR);
  Wire.write(0x03); // Start reading
  Wire.endTransmission();

  Wire.requestFrom(HMC5883L_ADDR, 6);

  if (Wire.available() == 6) {
    x = Wire.read() << 8 | Wire.read();
    z = Wire.read() << 8 | Wire.read();
    y = Wire.read() << 8 | Wire.read();
  }

  float heading = atan2(y, x);
  if (heading < 0) heading += 2 * PI;

  float headingDegrees = heading * 180 / PI;

  Serial.print("Heading: ");
  Serial.println(headingDegrees);

  delay(500);
}
