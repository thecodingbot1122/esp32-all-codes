const int ledPin = 9;  // LED control pin
int brightness = 255; 
void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  
    analogWrite(ledPin, brightness);
   
}
