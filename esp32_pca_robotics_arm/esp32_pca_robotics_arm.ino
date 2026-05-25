#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN 150
#define SERVOMAX 600
#define SERVO_FREQ 50

#define NUM_SERVOS 6
int servoChannels[NUM_SERVOS] = {10, 11, 12, 13, 14, 15};

int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);

  Serial.println("PCA9685 Ready (Mega)");
}

void loop() {

  // 🔵 Move servos ONE BY ONE (0 → 180)
  for (int i = 0; i < NUM_SERVOS; i++) {

    for (int angle = 0; angle <= 180; angle += 3) {
      int pulse = angleToPulse(angle);
      pwm.setPWM(servoChannels[i], 0, pulse);
      delay(15);
    }

    delay(300);
  }

  delay(500);

  // 🔴 Move servos ONE BY ONE (180 → 0)
  for (int i = 0; i < NUM_SERVOS; i++) {

    for (int angle = 180; angle >= 0; angle -= 3) {
      int pulse = angleToPulse(angle);
      pwm.setPWM(servoChannels[i], 0, pulse);
      delay(15);
    }

    delay(300);
  }

  delay(500);
}
