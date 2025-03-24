#define LED_PIN 2       // Built-in LED on ESP32
#define TRIG_PIN 5      // Ultrasonic Sensor Trig Pin
#define ECHO_PIN 18     // Ultrasonic Sensor Echo Pin

void TaskBlink(void *pvParameters) {
    pinMode(LED_PIN, OUTPUT);
    while (1) {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("LED ON");   // Print status to Serial Monitor
        vTaskDelay(500 / portTICK_PERIOD_MS);
        
        digitalWrite(LED_PIN, LOW);
        Serial.println("LED OFF");  // Print status to Serial Monitor
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void TaskUltrasonic(void *pvParameters) {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    while (1) {
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);

        long duration = pulseIn(ECHO_PIN, HIGH);
        long distance = duration * 0.034 / 2;

        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" cm");

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(9600); // Set communication baud rate to 9600
    Serial.println("ESP32 FreeRTOS Starting...");

    // Run LED Blink Task on Core 0
    xTaskCreatePinnedToCore(TaskBlink, "BlinkTask", 1000, NULL, 1, NULL, 0);
    
    // Run Ultrasonic Sensor Task on Core 1
    xTaskCreatePinnedToCore(TaskUltrasonic, "UltraTask", 1000, NULL, 1, NULL, 1);
}

void loop() {
    // Empty because FreeRTOS handles tasks
}
