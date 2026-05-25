// L298N Motor Driver Test Code (2 Motors)

#define ENA 9
#define IN1 8
#define IN2 7

#define ENB 10
#define IN3 6
#define IN4 5

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.begin(9600);
  Serial.println("L298N Motor Driver Test Started");
}

void loop() {

  // ---------- MOTOR A FORWARD ----------
  Serial.println("Motor A Forward");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 200);

  // ---------- MOTOR B FORWARD ----------
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENB, 200);

  delay(3000);

  // ---------- STOP BOTH ----------
  Serial.println("Stop Both Motors");
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  delay(2000);

  // ---------- MOTOR A BACKWARD ----------
  Serial.println("Motor A Backward");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 200);

  // ---------- MOTOR B BACKWARD ----------
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENB, 200);

  delay(3000);

  // ---------- STOP BOTH ----------
  Serial.println("Stop Both Motors");
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  delay(3000);
}
