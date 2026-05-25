#include <Arduino.h>
#include <Servo.h>

Servo myServo;

int servoPin = 9;   // change pin if needed
int angle ;

void setup() {

  Serial.begin(115200);

  myServo.attach(servoPin);
  myServo.write(90);

  Serial.println("Enter servo angle between 0 and 180:");
}

void loop() {

  if (Serial.available() > 0) {

    angle = Serial.parseInt();   // read angle from serial

    if (angle > 0 && angle <= 180) {

      myServo.write(angle);      // move servo
      Serial.print("Servo moved to: ");
      Serial.println(angle);

    } 
    else {

      Serial.println("Invalid angle. Enter 0 - 90");

    }
  }
}
