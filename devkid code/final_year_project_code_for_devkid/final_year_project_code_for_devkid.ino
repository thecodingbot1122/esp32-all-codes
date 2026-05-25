/*********
  Project: ESP32 Robot - Manual & Auto Control
  Features: PCA9685 Servo Control, L298N Motor Control, Instant Pause/Resume
*********/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// --- PCA9685 Servo Configuration ---
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
#define SERVOMIN 150   
#define SERVOMAX 600   
#define SERVO_FREQ 50  

// Servo Channels
#define NUM_SERVOS 7
int servoChannels[NUM_SERVOS] = {0, 10, 11, 12, 13, 14, 15};
int initialAngles[7] = {60, 90, 70, 40, 120, 60, 140};
int pos,count=0,baseservo=0;

// --- Motor Pin Definitions ---
#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33
#define ENA 14
#define ENB 32

// --- Sensor Pins ---
const int analogPin = 13;

// --- State Variables ---
HardwareSerial mySerial(2);
bool autoModeActive = false;  // Managed by 'X' (Start) and 'Z' (Pause)
int currentScanPos = 0;       // Remembers where the scanner is
bool sweepDirectionUp = true; // For the back-and-forth motion
String message = "";


// --- Helper Functions ---
int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

//input function:
void READMESSAGE( ){
if (mySerial.available()) {
    message = mySerial.readStringUntil('\n');
    message.trim();
    }
    }

                                ///Menual car 
void meualcar()
{
  if (message == "F") {carforwardA();}

else if (message == "B") {carbackwardA();}

else if (message == "L") {carleft();}

else if (message == "R") {carright();}

else if (message == "S") {carstop();}
}

//menuwal servo movement
void menualservo( )
{
    if (message.indexOf(':') != -1) {
      int colonIndex = message.indexOf(':');
      String id = message.substring(0, colonIndex);
      int angle = message.substring(colonIndex + 1).toInt();
      
      Serial.print("Moving "); Serial.print(id); 
      Serial.print(" to "); Serial.println(angle);

      // Your Channel Mapping
      if (id == "S0")      pwm.setPWM(servoChannels[6], 0, angleToPulse(angle));
      else if (id == "S1") pwm.setPWM(servoChannels[5], 0, angleToPulse(angle));
      else if (id == "S2") pwm.setPWM(servoChannels[4], 0, angleToPulse(angle));
      else if (id == "S3") pwm.setPWM(servoChannels[3], 0, angleToPulse(angle));
      else if (id == "S4") pwm.setPWM(servoChannels[2], 0, angleToPulse(angle));
      else if (id == "S5") pwm.setPWM(servoChannels[1], 0, angleToPulse(angle));
    }
}

//menuwal scan
void runMenualScanStep() {
  // Serial.println(count);
  for(int i = 0 ; i<=130; i++){
   
    READMESSAGE( );
    
    meualcar();
    menualservo( );
   
    
  pwm.setPWM(baseservo, 0, angleToPulse(i));
  
  // Real-time sensor check
  int val = analogRead(analogPin);
  //Serial.println(val);
   if (val > 3000) {
    message="STOP";
    Serial.print("Metal detected");
      carstop();

      // Set final position based on angle
      if (i <= 90){
        pwm.setPWM(baseservo, 0, angleToPulse(130));}
      else{
        pwm.setPWM(baseservo, 0, angleToPulse(0));}
        delay(5000);

      return; // Exit function entirely when sensor triggers
    }
    message="SCAN";
    delay(5); // Smooth sweep
  }


  for(int i = 130 ; i>=0; i--){
  
   READMESSAGE( );
   
    meualcar();
    menualservo( );
    
  pwm.setPWM(baseservo, 0, angleToPulse(i));
  
  // Real-time sensor check
  int val = analogRead(analogPin);
  //Serial.println(val);
   if (val > 3000) {
      message="STOP";
      carstop();

      // Set final position based on angle
      if (i <= 90){
        pwm.setPWM(baseservo, 0, angleToPulse(130));
      }
      else{
        pwm.setPWM(baseservo, 0, angleToPulse(0));
        }
       delay(5000);
      return; // Exit function entirely when sensor triggers
    }
     message="SCAN";
   delay(5); // Smooth sweep
  }
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


//MANUWAL CAR MOVEMENT SECTION

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



                                               //automatic section

//for base movement 
void base ( )
{
  
    pwm.setPWM(servoChannels[1], 0, angleToPulse(pos));
}

//for jar movement
void jaropen( )
{
  for (int i =150 ; i>=80; i--)
  {
    pwm.setPWM(servoChannels[6], 0, angleToPulse(i));
    delay(100);
  }
}

void jarclose( )
{
  for (int i =80 ; i<=180; i++)
  {
    pwm.setPWM(servoChannels[6], 0, angleToPulse(i));
    delay(100);
  }
}

//for Servo2
void servo2up( )
{
  for (int i =180 ; i>=0; i--)
  {
    pwm.setPWM(servoChannels[4], 0, angleToPulse(i));
    delay(100);
  }
}

void servo2down( )
{
  for (int i =0 ; i<=180; i++)
  {
    pwm.setPWM(servoChannels[4], 0, angleToPulse(i));
    delay(100);
  }
}

//for Servo3
void servo3up( )
{
  for (int i =0 ; i<=180; i++)
  {
    pwm.setPWM(servoChannels[3], 0, angleToPulse(i));
    delay(100);
  }
}

void servo3down( )
{
  for (int i =180 ; i>=180; i--)
  {
    pwm.setPWM(servoChannels[3], 0, angleToPulse(i));
    delay(100);
  }
}

//for Servo4
void servo4up( )
{
  for (int i =0 ; i<=180; i++)
  {
    pwm.setPWM(servoChannels[2], 0, angleToPulse(i));
    delay(100);
  }
}

void servo4down( )
{
  for (int i =180 ; i>=180; i--)
  {
    pwm.setPWM(servoChannels[2], 0, angleToPulse(i));
    delay(100);
  }
}

// pick and drop automatic 
void pickanddrop( )
{
    base ( );
    jaropen( );
  
}

                                                //automatic scan
void runAutomaticScanStep() {
  // Serial.println(count);
  for(int i = 0 ; i<=130; i++){
    pos =i;
    
   READMESSAGE( );
  if(message == "Z")
   {
    return;
   }
   
   
  pwm.setPWM(baseservo, 0, angleToPulse(i));
  
  // Real-time sensor check
  int val = analogRead(analogPin);
  //Serial.println(val);
   if (val > 3000) {
      Serial.print("Metal detected");
      carstop();

      // Set final position based on angle
      if (i <= 90){
        pwm.setPWM(baseservo, 0, angleToPulse(130));
      }
      else{
        pwm.setPWM(baseservo, 0, angleToPulse(0));
        }
      pickanddrop( );
    }
    
    delay(5); // Smooth sweep
  }


  for(int i = 130 ; i>=0; i--){
    pos = i;
    
    READMESSAGE( );
    if(message == "Z")
   {
    return;
   }
    
  pwm.setPWM(baseservo, 0, angleToPulse(i));
  
  // Real-time sensor check
  int val = analogRead(analogPin);
  //Serial.println(val);
   if (val > 3000) {
      Serial.print("Metal detected");
      carstop();

      // Set final position based on angle
      if (i <= 90){
        pwm.setPWM(baseservo, 0, angleToPulse(130));
      }
      else{
        pwm.setPWM(baseservo, 0, angleToPulse(0));
        }
      pickanddrop( );
    }
     
    delay(5); // Smooth sweep
  }
}




//AUTOMATIC CAR MOVEMENT
void carforwardA() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  analogWrite(ENA, 200);  analogWrite(ENB, 200);
}

void carbackwardA() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, 150);   analogWrite(ENB, 150);
}

void carleftA() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 150);   analogWrite(ENB, 150);
}

void carrightA() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  analogWrite(ENA, 150);   analogWrite(ENB, 150);
}


// --- Setup ---
void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, 21, 19); // RX Pin 21, TX Pin 19
  Wire.begin(22, 23);
  
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);

  // Set initial servo positions from array
  for(int i = 0; i < NUM_SERVOS; i++) {
    pwm.setPWM(servoChannels[i], 0, angleToPulse(initialAngles[i]));
    delay(50);
  }

  // Initialize Motor Pins
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);
  
  carstop();
  Serial.println("Robot Online. Waiting for commands...");
}

// --- Main Loop ---
void loop() {
  // 1. Process Incoming Commands
 
  if (mySerial.available()) {
    message = mySerial.readStringUntil('\n');
    message.trim();
    Serial.print("Received: "); Serial.println(message);}

     //MANUAL SECTION
     //CAR MOVEMENT
      meualcar();

  // --- MANUAL SERVO LOGIC (Put it HERE) ---
         menualservo( );

//to start scan 

if (message == "SCAN")
{
  runMenualScanStep();
  Serial.println( message);
}
   



//automatic section

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

  // 2. Run Auto Scanning if active
    // Flow Control
    if (message == "Y") {
      
      Serial.println("AUTO START");
       runAutomaticScanStep();
  }
    
    
    else if (message == "Z") {
      carstop();
    for(int i = 0; i < NUM_SERVOS; i++) {
    pwm.setPWM(servoChannels[i], 0, angleToPulse(initialAngles[i]));
    delay(50);
  }
      Serial.println("AUTO PAUSE");
    }

    
 
}
