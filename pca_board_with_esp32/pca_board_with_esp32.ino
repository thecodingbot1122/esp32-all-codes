#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// PCA9685 object
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// Servo configuration
#define SERVOMIN 150   // 0 degrees
#define SERVOMAX 600   // 180 degrees
#define SERVO_FREQ 50  // 50Hz

#define NUM_SERVOS 6
int servoChannels[NUM_SERVOS] = {0, 1, 2, 3, 4, 5};

// Convert angle to PWM pulse
int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

void setup() {
  Serial.begin(115200);

  // I2C for ESP32
  Wire.begin(22, 23); // SDA, SCL

  // Start PCA9685
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);

  Serial.println("PCA9685 Ready - One by One Sweep!");
}

void loop() {
  // 🔵 Sweep each servo forward (0 → 180)
  for (int i = 0; i < NUM_SERVOS; i++) {         // Select servo
    for (int angle = 0; angle <= 180; angle += 3) { // Move servo fully
      pwm.setPWM(servoChannels[i], 0, angleToPulse(angle));
      delay(15); // Smooth step
    }
    delay(300); // Small pause after each servo finishes
  }

  delay(500); // Pause at 180

  // 🔴 Sweep each servo backward (180 → 0)
  for (int i = 0; i < NUM_SERVOS; i++) {         // Select servo
    for (int angle = 180; angle >= 0; angle -= 3) { // Move servo fully
      pwm.setPWM(servoChannels[i], 0, angleToPulse(angle));
      delay(15); // Smooth step
    }
    delay(300); // Small pause after each servo finishes
  }

  delay(500); // Pause at 0
}
