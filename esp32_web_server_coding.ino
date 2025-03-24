#include <WiFi.h>
#include <WebServer.h>

#define WIFI_SSID "K2HOUSE"
#define WIFI_PASSWORD "House@12"

const int LED1 = 26;
const int LED2 = 27;

bool led1State = false;
bool led2State = false;

WebServer server(80);

void sendHtml() {
  String response = R"rawliteral(
    <!DOCTYPE html>
    <html>
      <head>
        <title>ESP32 Web Server</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
          body { font-family: Arial, sans-serif; text-align: center; margin: 50px; }
          .btn { font-size: 20px; padding: 10px; margin: 10px; cursor: pointer; border: none; }
          .on { background-color: green; color: white; }
          .off { background-color: red; color: white; }
        </style>
      </head>
      <body>
        <h1>ESP32 Web Server</h1>
        <button class="btn LED1_CLASS" onclick="location.href='/toggle/1'">LED 1: LED1_TEXT</button>
        <button class="btn LED2_CLASS" onclick="location.href='/toggle/2'">LED 2: LED2_TEXT</button>
      </body>
    </html>
  )rawliteral";

  response.replace("LED1_TEXT", led1State ? "ON" : "OFF");
  response.replace("LED2_TEXT", led2State ? "ON" : "OFF");
  response.replace("LED1_CLASS", led1State ? "on" : "off");
  response.replace("LED2_CLASS", led2State ? "on" : "off");

  server.send(200, "text/html", response);
}

void setup() {
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
   Serial.println("outside loop");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
     Serial.println("inside loop");
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", sendHtml);
  server.on("/toggle/1", []() {
    led1State = !led1State;
    digitalWrite(LED1, led1State);
    sendHtml();
  });
  
  server.on("/toggle/2", []() {
    led2State = !led2State;
    digitalWrite(LED2, led2State);
    sendHtml();
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
