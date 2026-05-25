#include <WiFi.h>
#include <ESP32Servo.h>

// WiFi credentials
const char* ssid = "Hostel one";
const char* password = "hostel123";

// Web server
WiFiServer server(80);

// Create 6 servo objects
Servo servos[6];

// Servo pins (change if needed)
int servoPins[6] = {13, 14, 15, 16, 17, 18};

// Store slider values
String servoValue[6] = {"90","90","90","90","90","90"};

// HTTP request
String header;

// Timing
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);

  // Attach all servos
  for(int i = 0; i < 6; i++){
    servos[i].attach(servoPins[i]);
    servos[i].write(90); // initial position
  }

  // Connect WiFi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    currentTime = millis();
    previousTime = currentTime;

    String currentLine = "";

    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();

      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;

        if (c == '\n') {
          if (currentLine.length() == 0) {

            // ----------- HANDLE SERVO VALUES -----------
            for(int i = 0; i < 6; i++){
              String key = "GET /?s" + String(i) + "=";

              if (header.indexOf(key) >= 0) {
                int p1 = header.indexOf('=');
                int p2 = header.indexOf('&');
                String val = header.substring(p1 + 1, p2);

                servoValue[i] = val;
                servos[i].write(val.toInt());

                Serial.print("Servo ");
                Serial.print(i + 1);
                Serial.print(" = ");
                Serial.println(val);
              }
            }

            // ----------- SEND WEB PAGE -----------
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            client.println("<!DOCTYPE html><html>");
            client.println("<head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<style>");
            client.println("body { text-align:center; font-family: Arial; }");
            client.println(".slider { width: 300px; }");
            client.println("</style>");
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
            client.println("</head><body>");

            client.println("<h2>ESP32 - 6 Servo Control</h2>");

            // Create sliders
            for(int i = 0; i < 6; i++){
              client.println("<p>Servo " + String(i+1) + ": <span id='val"+String(i)+"'>" + servoValue[i] + "</span></p>");
              client.println("<input type='range' min='0' max='180' value='" + servoValue[i] + "' class='slider' ");
              client.println("oninput='update"+String(i)+"(this.value)'>");
            }

            // JavaScript
            client.println("<script>");

            for(int i = 0; i < 6; i++){
              client.println("function update"+String(i)+"(val){");
              client.println("document.getElementById('val"+String(i)+"').innerHTML = val;");
              client.println("$.get('/?s"+String(i)+"=' + val + '&');");
              client.println("}");
            }

            client.println("</script>");

            client.println("</body></html>");
            client.println();

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    // Clear header
    header = "";

    // Close connection
    client.stop();
    Serial.println("Client disconnected.\n");
  }
}
