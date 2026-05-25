#include <QMC5883LCompass.h>

QMC5883LCompass compass;

void setup() {
  Serial.begin(9600);
  
  // Initialize the sensor (defaults to address 0x0D)
  compass.init();
  
  // Optional: Set calibration values if you have them. 
  // For now, let's just see if the numbers move!
  Serial.println("QMC5883L Detected! Rotate the sensor...");
}

void loop() {
  // Read the sensor data
  compass.read();
  
  // Get the Azimuth (Heading)
  // 0 = North, 90 = East, 180 = South, 270 = West
  int heading = compass.getAzimuth();

// If the heading is negative, add 360 to make it positive

//if (heading < 0) { heading += 360;}

  
  // Also get raw values to see if it's alive
  int x = compass.getX();
  int y = compass.getY();
  int z = compass.getZ();

  Serial.print("Heading: ");
  Serial.print(heading);
  Serial.print(" | X: ");
  Serial.print(x);
  Serial.print(" Y: ");
  Serial.println(y);

  delay(250);
}
