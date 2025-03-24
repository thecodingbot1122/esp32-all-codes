#define LED1 2  // First LED on GPIO 2
#define LED2 4  // Second LED on GPIO 4

void setup() {
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
}

void loop() {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    delay(500);

    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    delay(500);
}
