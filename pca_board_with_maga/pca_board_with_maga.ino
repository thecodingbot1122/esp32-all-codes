#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Standard servo pulse settings (may need minor adjustment for your specific servo)
#define SERVOMIN  150 // Minimum pulse length (approx 0 degrees)
#define SERVOMAX  600 // Maximum pulse length (approx 180 degrees)
#define SERVO_FREQ 50 // Analog servos run at 50Hz

void setup() {
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);
}

void loop() {
  // 0 to 180 degrees
  for (int angle = 0; angle <= 180; angle +=3) {
    int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX); // Convert degrees to ticks
    pwm.setPWM(0, 0, pulse); // Move servo on Channel 0
    delay(15); // Wait 15ms for movement
  }

  delay(500); // Brief pause at 180

  // 180 back to 0 degrees
  for (int angle = 180; angle >= 0; angle -= 3) {
    int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
    pwm.setPWM(0, 0, pulse);
    delay(15);
  }

  delay(500); // Brief pause at 0
}
