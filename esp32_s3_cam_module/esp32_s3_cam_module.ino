#include <WiFi.h>
#include <esp_camera.h>

// Replace with your network credentials
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// Camera configuration
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    16
#define SIOD_GPIO_NUM    21
#define SIOC_GPIO_NUM    22
#define Y9_GPIO_NUM      12
#define Y8_GPIO_NUM      13
#define Y7_GPIO_NUM      14
#define Y6_GPIO_NUM      15

void startCameraServer();

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Wait for Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Initialize the camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y9_GPIO_NUM;
  config.pin_d1 = Y8_GPIO_NUM;
  config.pin_d2 = Y7_GPIO_NUM;
  config.pin_d3 = Y6_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (!esp_camera_init(&config)) {
    Serial.println("Camera init failed");
    return;
  }

  startCameraServer();
}

void loop() {
  // Main loop does nothing; camera server handles requests
}
