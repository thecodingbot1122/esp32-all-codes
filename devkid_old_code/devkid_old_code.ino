update this  code 
/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete instructions at https://RandomNerdTutorials.com/esp32-uart-communication-serial-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/
//for servos 
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// PCA9685 object
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// Servo configuration
#define SERVOMIN 150   // 0 degrees
#define SERVOMAX 600   // 180 degrees
#define SERVO_FREQ 50  // 50Hz

#define NUM_SERVOS 7
int servoChannels[NUM_SERVOS] = {0,10, 11, 12, 13, 14, 15};

// Convert angle to PWM pulse
int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}



// Define TX and RX pins for UART (change if needed)
#define TXD1 19
#define RXD1 21

// Use Serial1 for UART communication
HardwareSerial mySerial(2);

//for car movement

// Motor pins
#define IN1 27   // Left Motor
#define IN2 26   // Left Motor
#define IN3 25   // Right Motor
#define IN4 33   // Right Motor

// Enable pins
#define ENA 14   // Left Motor Enable
#define ENB 32   // Right Motor Enable

//for ground scanning
int servoPin = 12; // GPIO connected to servo
// Analog input setup
int analogPin = 13; // GPIO connected to analog sensor (0-4095)
int analogVal = 0,count=0;


void carforward() {
  Serial.println("Car moving forward");
   digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  
}

void carbackward() {
  Serial.println("Car moving backward");
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void carleft() {
  Serial.println("Car turning left");
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void carright() {
  Serial.println("Car turning right");
   digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void carstop() {
  Serial.println("Car stopped");
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

//scaning the ground 
void automaticscaning( )
{
    for (int pos = 0; pos <= 130; pos += 3) {
    pwm.setPWM(0, 0, angleToPulse(pos));
    delay(15);
    // Check analog during sweep
    analogVal = analogRead(analogPin);
    Serial.println(analogVal); 
     
     while(analogVal > 3000)
    { 
      //to keep servo at place
      carstop();
      pwm.setPWM(0, 0, angleToPulse(pos));
      Serial.println(analogVal);
      delay(3000);
      //Serial.println(pos);

      //moving scaner to pic things
      if(pos <=90)
      {
        pos =130;
        pwm.setPWM(0, 0, angleToPulse(pos));
        count++;
        delay(15);
      }

      if(pos >90)
      {
        pos =0;
        pwm.setPWM(0, 0, angleToPulse(pos));
        count++;
        delay(15);
      }

      delay(3000);
      break;
      
    } 

    if(count >0 )
    {
      count = 0;
      break;
    }
    
    }

    for (int pos = 130; pos >= 0; pos -= 3) {
    pwm.setPWM(0, 0, angleToPulse(pos));
    delay(15);
    // Check analog during sweep
    analogVal = analogRead(analogPin);
    Serial.println(analogVal);
    
     while(analogVal > 3000)
    { 
      //to keep servo at place
      carstop();
      pwm.setPWM(0, 0, angleToPulse(pos));
      Serial.println(analogVal);
      delay(3000);
      //Serial.println(pos);

      //moving scaner to pic things
      if(pos <=90)
      {
        pos =130;
        pwm.setPWM(0, 0, angleToPulse(pos));
        count++;
        delay(15);
      }

      if(pos >90)
      {
        pos =0;
        pwm.setPWM(0, 0, angleToPulse(pos));
        count++;
        delay(15);
      }

      delay(3000);
      break;
      
    } 

    if(count >0 )
    {
      count = 0;
      break;
    }
   
  }
  
}

//automatic pic up
int angles1 [5] ={ 90,90,90,90,90};
void  automaticservomoment( )
{
 for(int i = 1; i < 6; i++) {
  
  for(int j=0; j<=angles1[i];j++)
  {
  pwm.setPWM(servoChannels[i], 0, angleToPulse(angles1[j]));
  
  Serial.println(servoChannels[i]);
  Serial.println(angles1[i]);
  
  delay(20);
  }
}
  
}

void setup() {
  Serial.begin(115200);
  
  // I2C for ESP32
  Wire.begin(22, 23); // SDA, SCL

  // Start PCA9685
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);
  Serial.println("servo angle setting");
  
  int angles[7] = {50,90, 90, 50, 130, 60, 130};
   
  for(int i = 0; i < 8; i++) {
  pwm.setPWM(servoChannels[i], 0, angleToPulse(angles[i]));
  
  Serial.println(servoChannels[i]);
  Serial.println(angles[i]);
  
  delay(20);
}

  //car section
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Enable motors (full speed)
  digitalWrite(ENA, 200);
  digitalWrite(ENB, 200);

   carstop();
  Serial.println("ESP32 Bluetooth Car Ready (EN Pins Enabled)");

  // UART setup
  mySerial.begin(9600, SERIAL_8N1, RXD1, TXD1);  
  
  Serial.println("ESP32 UART Receiver");
}

void loop()
{  
   Serial.println("scanning the area");
  // Check if data is available to read
  if (mySerial.available()) {
    // Read data and display it
    String message = mySerial.readStringUntil('\n');
    Serial.println("Received: " + message);
        message.trim();
   
  //for menual 
   while(message != "A"){
   
   if (mySerial.available()) {
    // Read data and display it
    String message = mySerial.readStringUntil('\n');
   // Serial.println("Received: " + message);
        message.trim();
 
//servo section     
  // 1. Find the position of the colon ':'
  int colonIndex = message.indexOf(':');

  if (colonIndex != -1) {
    // 2. Extract the ID (everything before the colon)
    String servoID = message.substring(0, colonIndex); 

    // 3. Extract the Angle (everything after the colon)
    String angleString = message.substring(colonIndex + 1);
    
    // 4. Convert the string angle to an actual integer
    int angleValue = angleString.toInt(); 

    // Example: Move a specific servo based on ID
    if (servoID == "S0") {
     Serial.println("servo 0 moved: "); // This prints the text
     Serial.println(angleValue);      // This prints the actual integer
     pwm.setPWM(servoChannels[5], 0, angleToPulse(angleValue));
      //delay(15); // Smooth step
    }
    
    if (servoID == "S1") {
     Serial.println("servo 1 moved: "); // This prints the text
     Serial.println(angleValue);      // This prints the actual integer
     pwm.setPWM(servoChannels[4], 0, angleToPulse(angleValue));
     // delay(15); // Smooth step
    }
    
    if (servoID == "S2") {
     Serial.println("servo 2 moved: "); // This prints the text
     Serial.println(angleValue);      // This prints the actual integer
     pwm.setPWM(servoChannels[3], 0, angleToPulse(angleValue));
    // delay(15); // Smooth step
    }
    
    if (servoID == "S3") {
     Serial.println("servo 3 moved: "); // This prints the text
     Serial.println(angleValue);      // This prints the actual integer
     pwm.setPWM(servoChannels[2], 0, angleToPulse(angleValue));
    // delay(15); // Smooth step
    }

    if (servoID == "S4") {
     Serial.println("servo 4 moved: "); // This prints the text
     Serial.println(angleValue);      // This prints the actual integer
     pwm.setPWM(servoChannels[1], 0, angleToPulse(angleValue));
    // delay(15); // Smooth step
    }

    if (servoID == "S5") {
     Serial.println("servo 5 moved: "); // This prints the text
     Serial.println(angleValue);      // This prints the actual integer
     pwm.setPWM(servoChannels[0], 0, angleToPulse(angleValue));
     delay(15); // Smooth step
    }
  }

//carsection
    if (message == "F") {
      carforward();
    } 
    else if (message == "B") {
      carbackward();
    } 
    else if (message == "L") {
      carleft();
    } 
    else if (message == "R") {
      carright();
    } 
    else if (message == "S") {
      carstop();
    }
  }
  delay(100);
  }

//for automatic 
  // 1. Initial check (assuming message was already set to something else)
while (message != "M") {
  //Serial.println("In automatic section...");
 
  if (mySerial.available()) {
    // REMOVE 'String' here to update the global variable
    message = mySerial.readStringUntil('\n'); 
    message.trim(); 
  
  int colonIndex = message.indexOf(':');
  int commaIndex = message.indexOf(',');

  if (colonIndex != -1 && commaIndex != -1) {
    // 1. Get the ID (e.g., "G1", "G2", "G3", "G4")
    String id = message.substring(0, colonIndex);

    // 2. Get the Latitude (Value 1)
    long lat = message.substring(colonIndex + 1, commaIndex).toInt();

    // 3. Get the Longitude (Value 2)
    long lon = message.substring(commaIndex + 1).toInt();

    // 4. Decide what to do based on the ID
    if (id == "G1") {
      Serial.println("Point 1 Received: Lat " + String(lat) + " Lon " + String(lon));
      // move_to_point(lat, lon); 
    } 
    else if (id == "G2") {
      Serial.println("Point 2 Received: Lat " + String(lat) + " Lon " + String(lon));
    }
    else if (id == "G3") {
       Serial.println("Point 3 Received: Lat " + String(lat) + " Lon " + String(lon));
    }
    else if (id == "G4") {
       Serial.println("Point 4 Received: Lat " + String(lat) + " Lon " + String(lon));
    }
  }

  
 

//press start button to start
if(message =="X")
{ 
  //automatic scanning
  automaticscaning( );
  automaticservomoment( );
 
}

//press start button to start
if(message =="Z")
{ 

  break;
 
}

   
  // Add a small delay so the Serial Monitor doesn't lag
  delay(100); 
  }
  
}

}
}

    
