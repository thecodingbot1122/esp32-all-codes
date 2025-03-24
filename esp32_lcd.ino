#include <LiquidCrystal.h>

// Define LCD pins
const int RS = 21, E = 22, D4 = 19, D5 = 18, D6 = 5, D7 = 4;
const int contrastPin = 15;  // PWM pin for contrast

// Initialize the LCD (4-bit mode)
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

void setup() {
  lcd.begin(16, 2);  // Initialize 16x2 LCD
  lcd.print("Hello, ESP32!");

  // Setup PWM for contrast control
  ledcSetup(0, 5000, 8);  // Channel 0, 5kHz frequency, 8-bit resolution
  ledcAttachPin(contrastPin, 0);
  ledcWrite(0, 50);  // Adjust value (0-255) for contrast
}

void loop() {
  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000); // Display uptime in seconds
  delay(1000);
}
