/* ESP32-S3 SENDER CODE */
#define S3_RX 18
#define S3_TX 17

int myNumber = 0;

void setup() {
  Serial.begin(115200); // For Serial Monitor (USB)
  
  // Initialize UART1 with custom pins
  Serial1.begin(9600, SERIAL_8N1, S3_RX, S3_TX);
  
  Serial.println("S3 Sender Started...");
}

void loop() {
  Serial.print("Sending Number: ");
  Serial.println(myNumber);

  // Send the integer as a string with a newline
  Serial1.println(myNumber); 
  
  myNumber++; // Increment the number
  delay(1000); 
}
