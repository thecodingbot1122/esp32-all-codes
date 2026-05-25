#include <Arduino.h>
/* ESP32 DEVKIT RECEIVER */
#define DEV_RX 14
#define DEV_TX 27

void setup() {
  Serial.begin(115200); // USB Debugging
  
  // Start UART2: 9600 baud, Custom Pins 14 & 27
  Serial2.begin(9600, SERIAL_8N1, DEV_RX, DEV_TX);
  
  // Optimization: Don't wait 1 second for numbers to finish
  Serial2.setTimeout(10); 
  
  Serial.println("DevKit Receiver Ready!");
}

void loop() {
  // Check if data is waiting
  if (Serial2.available() > 0) {
    
    // 1. Read the first character (F or B)
    char cmd = Serial2.read();
    
    // 2. Automatically skip the comma and find the integer
    int value = Serial2.parseInt();
    
    // 3. Clear the buffer of any leftover newline characters (\n)
    while(Serial2.available() > 0 && (Serial2.peek() == '\n' || Serial2.peek() == '\r')) {
      Serial2.read();
    }

    // 4. Use the data!
    Serial.print("--- New Packet ---");
    Serial.print(" Command: "); Serial.println(cmd);
    Serial.print(" Value: ");   Serial.println(value);
    
    // Example: Logic based on the character
    if (cmd == 'F') {
      Serial.println("Action: Moving FORWARD at speed " + String(value));
    } else if (cmd == 'B') {
      Serial.println("Action: Moving BACKWARD at speed " + String(value));
    }
    Serial.println();
  }
}
