#include <ESP32Servo.h>

// Create servo object
Servo myservo;  

int servoPin = 5; // GPIO pin connected to servo

void setup() {
  // Allocate ESP32 hardware timers for PWM (safe for multiple servos)
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // Set PWM frequency for standard servo (50 Hz)
  myservo.setPeriodHertz(50);

  // Attach servo to pin with min/max pulse widths (in microseconds)
  myservo.attach(servoPin, 500, 2400); 
  Serial.begin(115200);
  myservo.write(20);
}

void loop() {

 // Sweep from 0° to 180°
  for (int pos = 20; pos <= 180; pos += 20) {
    myservo.write(pos); // Move servo to position
    Serial.println(pos);
    delay(5);          // Small delay for smooth motion
  }

  // Sweep back from 180° to 0°
  for (int pos = 180; pos >= 20; pos -= 20) {
    myservo.write(pos);
     Serial.println(pos);
    delay(5);
  }
}
