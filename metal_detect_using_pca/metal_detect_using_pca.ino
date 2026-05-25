
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// --- PCA9685 Servo Configuration ---
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
#define SERVOMIN 150   
#define SERVOMAX 600   
#define SERVO_FREQ 50 

int servoChannel = 0;   // PCA9685 channel connected to servo
int analogPin = 13;     // Analog sensor pin
int analogVal = 0;

// Convert 0-180 degree angle to PWM pulse
int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

void setup() {
  Serial.begin(115200);

  // Initialize I2C with SDA = 22, SCL = 23
  Wire.begin(22, 23);

  // Initialize PCA9685
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);  // 50Hz

  // Set servo to initial 0°
  pwm.setPWM(servoChannel, 0, angleToPulse(0));
  delay(500);
}

void loop() {
  // --- Forward Sweep (0 → 180) ---
  for (int pos = 0; pos <= 180; pos += 3) {
    pwm.setPWM(servoChannel, 0, angleToPulse(pos));
    delay(10);

    // Read analog sensor
    analogVal = analogRead(analogPin);
    Serial.println(analogVal);

    // Hold servo if sensor value > 3000
    while (analogVal > 3000) {
      analogVal = analogRead(analogPin);
      pwm.setPWM(0, 0, angleToPulse(pos));
     // Serial.print("Holding at pos: "); Serial.println(pos);
     // Serial.print("Analog value: "); Serial.println(analogVal);
    }
  }

  // --- Backward Sweep (180 → 0) ---
  for (int pos = 180; pos >= 0; pos -= 3) {
    pwm.setPWM(0, 0, angleToPulse(pos));
    delay(15);

    // Read analog sensor
    analogVal = analogRead(analogPin);
    Serial.println(analogVal);

    // Hold servo if sensor value > 3000
    while (analogVal > 3000) {
      analogVal = analogRead(analogPin);
      pwm.setPWM(servoChannel, 0, angleToPulse(pos));
     // Serial.print("Holding at pos: "); Serial.println(pos);
     // Serial.print("Analog value: "); Serial.println(analogVal);
    }
  }
  delay(500);
}
