#include <ESP32Servo.h>

Servo servos[6], scanservo;
bool input;

// Servo pins
int servoPins[6] = {4, 5, 18, 19, 21, 22};

// Motor pins
#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33

#define ENA 14
#define ENB 32

int analogPin = 34;
int analogVal = 0;

// ================= SERVO =================
void servoforward()
{
  for(int i = 0; i < 6; i++)
  {
    for(int j = 0; j <= 180; j++)
    {
      servos[i].write(j);
      delay(20);
    }
  }
}

void servobackward()
{
  for(int i = 0; i < 6; i++)
  {
    for(int j = 180; j >= 0; j--)
    {
      servos[i].write(j);
      delay(20);
    }
  }
}

// ================= MOTOR =================
void forward() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopCar() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ================= SCANNING =================
bool scanning()
{
  for (int pos = 0; pos <= 180; pos += 8)
  {
    scanservo.write(pos);
    delay(15);

    analogVal = analogRead(analogPin);
    Serial.println(analogVal);

    if(analogVal > 3000)
    {
      Serial.println("Metal Detected!");
      return true;
    }
  }

  for (int pos = 180; pos >= 0; pos -= 8)
  {
    scanservo.write(pos);
    delay(15);

    analogVal = analogRead(analogPin);

    if(analogVal > 3000)
    {
      Serial.println("Metal Detected!");
      return true;
    }
  }

  return false; // ✅ VERY IMPORTANT
}

// ================= SETUP =================
void setup()
{
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  for(int i = 0; i < 6; i++)
  {
    servos[i].attach(servoPins[i]);
  }

  scanservo.attach(2);
}

// ================= LOOP =================
void loop()
{
  forward();

  input = scanning();   // ✅ only call once

  if (input)
  {
    stopCar();

    servoforward();
    servobackward();

    delay(1000);
  }
}
