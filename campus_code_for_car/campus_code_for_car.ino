#include <QMC5883LCompass.h>
#include <Wire.h>

QMC5883LCompass compass;
#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33
#define ENA 14    
#define ENB 32

int slowSpeed = 130;
int fastSpeed = 255;
void carleftA() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); 
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, fastSpeed); analogWrite(ENB, fastSpeed);
  Serial.println("calling left side");
}

// Delay() ki jagah non-blocking timer
unsigned long previousMillis = 0;
const long interval = 250; 

void setup() {
  Serial.begin(115200);
  // Yahan Motor pins ko OUTPUT declare karna sab se zaroori hai!
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  // ESP32 I2C pins
  Wire.begin(22, 23); 
  
  compass.init();
  
  // Aap ki final correct calibration values
  //compass.setCalibration(4437, 6382, -7937, -5801, -3641, -3137);
  compass.setCalibration(4036, 6213, -7885, -5492, -3267, -2691);
  
  Serial.println("Compass Calibrated! Ready for Navigation.");
  //carleftA();
}

int getHeading() {
  compass.read();
  int a = compass.getAzimuth();
  
  // Angle ko 0-359 ke darmiyan rakhne ki logic
  if (a < 0) {
    a = a + 360;
  }
  return a;
}

void loop() {
  
  unsigned long currentMillis = millis();

  // Har 250ms baad angle print hoga, ESP32 freeze nahi hoga
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    int currentAngle = getHeading();
    
    Serial.print("Heading Angle: ");
    Serial.print(currentAngle);
    Serial.println("°");
  }
  
  // Aap ki AutomaticCarMovement() aur web server ki logic yahan smoothly chal sakti hai
}
