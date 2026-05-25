#include <ESP32Servo.h>

// Servo setup
Servo myservo;  
int servoPin = 12; // GPIO connected to servo

// Analog input setup
int analogPin = 13; // GPIO connected to analog sensor (0-4095)
int analogVal = 0;

void setup() {
  Serial.begin(115200);

  // Allocate hardware timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  // Servo PWM settings
  myservo.setPeriodHertz(50);        // 50Hz standard
  myservo.attach(servoPin, 500, 2400); // min/max pulse width
}

void loop() {
  // Read analog sensor
  analogVal = analogRead(analogPin);
   // Optional: see values in serial monitor


  // Sweep servo if analog value ≤ 2000
  for (int pos = 0; pos <= 180; pos += 8) {
    myservo.write(pos);
    delay(15);

    // Check analog during sweep
    analogVal = analogRead(analogPin);
    Serial.println(analogVal);
    
    while(analogVal > 3000)
    { 
      analogVal = analogRead(analogPin);
      myservo.write(pos);
      Serial.println(pos);
      Serial.println(analogVal);
       
      
    }
   
  }

  for (int pos = 180; pos >= 0; pos -= 8) {
    myservo.write(pos);
    delay(15);

    // Check analog during sweep
    analogVal = analogRead(analogPin);
    Serial.println(analogVal);
     while(analogVal > 3000)
    { 
      analogVal = analogRead(analogPin);
      myservo.write(pos);
      Serial.println(pos);
       Serial.println(analogVal);
      
    }
  }
}
