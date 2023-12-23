#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 4

const char* ssid = "A1_A2FAE9"; //YourWiFiSSID
const char* password = "13605f52"; //YourWiFiPassword
const char* serverAddress = "http://192.168.1.3:5000/receive_data";

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);	

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensor(&oneWire);

void setup(void)
{
  
  sensor.begin();	// Start up the library
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void sendSensorData(float data) {
  HTTPClient http;
  // Your Flask server endpoint
  http.begin(serverAddress);
  // Specify content type
  http.addHeader("Content-Type", "application/json");
  // Create JSON payload
  String jsonData = "{\"sensorData\":" + String(data) + "}";
  // Send the POST request
  int httpResponseCode = http.POST(jsonData);
  // Check for errors
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("HTTP Error: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
}
void loop(void)
{
  // Initiate temperature reading
  sensor.requestTemperatures();

  // Wait for temperature reading to complete (adjust the delay if needed)
  delay(750);

  // Fetch the temperature value
  float temperatureC = sensor.getTempCByIndex(0);

  // Print the temperature in Celsius
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println("C");

  // Send data to Flask server
  sendSensorData(temperatureC);

  delay(5000); // Adjust the delay based on your requirements
}
