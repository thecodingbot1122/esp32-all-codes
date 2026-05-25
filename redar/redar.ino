
#include <Servo.h>
// HC-SR04 Ultrasonic Sensor Connection
const int trigPin = 10;
const int echoPin = 11;

// Variables for duration and distance
long duration;
int distanceCm, distanceInch;


Servo myservo;  // create Servo object to control a servo
// twelve Servo objects can be created on most boards

int pos = 80;    // variable to store the servo position

void setup() {
  myservo.attach(9); 
   pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  (echoPin, INPUT); // Sets the echoPin as an Input// attaches the servo on pin 9 to the Servo object
  myservo.write(pos);
  Serial.begin(9600);
}

void scan( )
{
    // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * 0.034 / 2;
  distanceInch = duration * 0.0133 / 2;
  
  // Print the distance to the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distanceCm);
  Serial.println(" cm");
  
  delay(100); // Short delay for stability
}

void loop() {
  for (pos = 0; pos <= 180; pos += 4) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);  
    // tell servo to go to position in variable 'pos'
    scan( );
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 4) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    scan( );
    delay(15);                       // waits 15 ms for the servo to reach the position
  }
}
