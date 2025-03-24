#include <WiFi.h>
#include <LiquidCrystal.h>
#include <time.h> // Required for time functions

// LCD pin definitions (Parallel Mode, No I2C)
const int rs = 21, en = 19, d4 = 5, d5 = 17, d6 = 16, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Contrast control using PWM
const int contrastPin = 18;

// WiFi and NTP settings
#define WIFI_SSID      "K2HOUSE"
#define WIFI_PASSWORD  "House@12"
#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET     0
#define UTC_OFFSET_DST 0

void spinner() {
  static int8_t counter = 0;
  const char* glyphs = "\xa1\xa5\xdb";
  lcd.setCursor(15, 1);
  lcd.print(glyphs[counter++]);
  if (counter == strlen(glyphs)) {
    counter = 0;
  }
}

void printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    lcd.setCursor(0, 1);
    lcd.print("Connection Err ");
    return;
  }

  lcd.setCursor(5, 0);
  lcd.printf("%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

  lcd.setCursor(0, 1);
  lcd.printf("%02d/%02d/%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(18, OUTPUT);
  analogWrite(18,150);

  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi..");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    spinner();
  }

  Serial.println("\nWiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Online");
  lcd.setCursor(0, 1);
  lcd.print("Updating time...");

  configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
}

void loop() {
  printLocalTime();
  delay(1000);
}
