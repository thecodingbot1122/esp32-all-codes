#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// PCA9685 object (I2C address 0x40)
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// 🔧 Servo calibration values (adjust if needed)
#define SERVOMIN 120   // Minimum pulse length
#define SERVOMAX 650   // Maximum pulse length

// 🎯 Convert angle (0–180) to PWM pulse
int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

void setup() {
  Serial.begin(115200);

  // 🔌 Manually define I2C pins
  Wire.begin(18, 19);  // SDA = 18, SCL = 19

  pwm.begin();
  pwm.setPWMFreq(50);  // 50Hz for servo

  delay(100);

  Serial.println("✅ PCA9685 Initialized");
}

void loop() {

  Serial.println("➡️ Moving Forward");

  // 🔁 Sweep 0° → 180°
  for (int angle = 0; angle <= 180; angle += 5) {
    pwm.setPWM(0, 0, angleToPulse(angle));  // Channel 0
    Serial.print("Angle: ");
    Serial.println(angle);
    delay(100);
  }

  delay(1000);

  Serial.println("⬅️ Moving Backward");

  // 🔁 Sweep 180° → 0°
  for (int angle = 180; angle >= 0; angle -= 5) {
    pwm.setPWM(0, 0, angleToPulse(angle));  // Channel 0
    Serial.print("Angle: ");
    Serial.println(angle);
    delay(100);
  }

  delay(1000);

}
