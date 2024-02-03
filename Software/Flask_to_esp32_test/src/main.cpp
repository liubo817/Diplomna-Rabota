#include <WiFi.h>

const char* ssid = "A1_A2FAE9"; //YourWiFiSSID
const char* password = "13605f52"; //YourWiFiPassword
const int serverPort = 80;

#define relayPin 19

WiFiServer server(serverPort);

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
  // Start the server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  // Check for incoming connections
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected");

    // Read the request from the client
    String request = client.readStringUntil('\r');
    Serial.println("Received request: " + request);

    // Handle the control signal based on the request
    if (request.indexOf("state=ON") != -1) {
      digitalWrite(relayPin, HIGH);
      Serial.println("Turning ON the LED");
    } else if (request.indexOf("state=OFF") != -1) {
      digitalWrite(relayPin, LOW);
      Serial.println("Turning OFF the LED");
    }

    // Send a response to the client
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("Control signal processed");

    // Close the connection
    client.stop();
    Serial.println("Client disconnected");
  }

  // Your main loop code here
}
