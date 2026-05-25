#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
SemaphoreHandle_t messageMutex;

// --- Configuration ---
#define SERVOMIN 150   
#define SERVOMAX 600   
#define SERVO_FREQ 50  
#define NUM_SERVOS 7
int servoChannels[NUM_SERVOS] = {0, 10, 11, 12, 13, 14, 15};

int angleToPulse(int angle) {
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}

void safePWM(int num, int on, int off) {
  if (xSemaphoreTake(messageMutex, portMAX_DELAY) == pdTRUE) {
    pwm.setPWM(num, on, off);
    xSemaphoreGive(messageMutex);
  }
}

void setup() {
  Serial.begin(115200);
  
  Wire.begin(22, 23); 
  Wire.setClock(400000);
  
  messageMutex = xSemaphoreCreateMutex(); // Mutex initialized
//  compass.init();
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);
  Serial.println("System Ready. Type S6:90 and press Enter.");
}

void loop() {
  if (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n');
    message.trim();

    if (message.length() > 0 && message.indexOf(':') != -1) {
      int colonIndex = message.indexOf(':');
      String id = message.substring(0, colonIndex);
      int angle = message.substring(colonIndex + 1).toInt();
      
      int targetChannel = -1;

      if (id == "S0")      targetChannel = servoChannels[6];
      else if (id == "S1") targetChannel = servoChannels[5];
      else if (id == "S2") targetChannel = servoChannels[4];
      else if (id == "S3") targetChannel = servoChannels[3];
      else if (id == "S4") targetChannel = servoChannels[2];
      else if (id == "S5") targetChannel = servoChannels[1];
      else if (id == "S6") targetChannel = servoChannels[0];

      if (targetChannel != -1) {
        Serial.print("Moving "); Serial.print(id);
        Serial.print(" on Channel "); Serial.print(targetChannel);
        Serial.print(" to Angle "); Serial.println(angle);
        safePWM(targetChannel, 0, angleToPulse(angle));
      }
    }
  }
}
