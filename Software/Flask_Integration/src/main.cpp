#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define ds18b20WaterPin 16
#define ds18b20Pin 34
#define uvPin 32
#define uvRefPin 12
#define turbidityPin 33
#define waterLevelSensorPower 21
#define waterLevelPin 26
#define brightnessPin 36

#define relayPin 19


int water_level;
int water_level_percent;


const char* ssid = ""; //YourWiFiSSID
const char* password = ""; //YourWiFiPassword
const char* serverAddress = "http://:5000/receive_data";
const int serverPort = 80;


WiFiServer server(serverPort);
OneWire oneWire(ds18b20WaterPin);	
DallasTemperature ds18b20_water(&oneWire);

OneWire oneWireAir(ds18b20Pin);	
DallasTemperature ds18b20(&oneWireAir);

void sendSensorData(const char* sensorType, float data) {
  HTTPClient http;

  // Check if WiFi is connected
  if (WiFi.status() == WL_CONNECTED) {
    // Your Flask server endpoint
    http.begin(serverAddress);
    // Specify content type
    http.addHeader("Content-Type", "application/json");
    // Create JSON payload
    String jsonData = "{\"sensorType\":\"" + String(sensorType) + "\",\"sensorData\":" + String(data) + "}";
    
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
  } else {
    Serial.println("WiFi not connected. Skipping HTTP request.");
  }

  delay(3000);
}




float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int turbidity()
{
  int sensorValue = analogRead(turbidityPin);
  int turbidity = map(sensorValue, 0, 4095, 0, 100);
  int clean = map(turbidity, 0, 100, 100, 0);
  return clean;
}


float waterTemp()
{
  ds18b20_water.requestTemperatures();
  delay(750);
  float temperatureC = ds18b20_water.getTempCByIndex(0);
  return temperatureC;
}

float airTemp()
{
  ds18b20.requestTemperatures();
  delay(750);
  float temperatureC = ds18b20.getTempCByIndex(0);
  return temperatureC;
}



float waterLevel()
{
  int waterLevel;
  int sensor_data;
  float waterLevelPercentage;
	digitalWrite(waterLevelSensorPower, HIGH);
	delay(10);
	waterLevel = analogRead(waterLevelPin);
	digitalWrite(waterLevelSensorPower, LOW);
  waterLevelPercentage = map(waterLevel, 0, 1400, 0, 100);
	return waterLevelPercentage;
  return 0;
}


float UVlevel()
{
  int uvLevel = analogRead(uvPin);
  int refLevel = analogRead(uvRefPin);
  float outputVoltage = 3.3 / refLevel * uvLevel;
  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); //Convert the voltage to a UV intensity level
  return uvIntensity;
}

int brightness()
{
  int brightnessLevel = analogRead(brightnessPin);
  int brightnessPercentage;
  brightnessPercentage = map(brightnessLevel, 0, 4095, 0, 100);
  return brightnessPercentage;
}



void setup(void)
{
  pinMode(waterLevelSensorPower, OUTPUT);
  pinMode(uvPin, INPUT);
  pinMode(uvRefPin, INPUT);
  pinMode(relayPin, OUTPUT);
  ds18b20_water.begin();
  ds18b20.begin();
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
  //Start the server
  server.begin();
  Serial.println("Server started");
}


void loop()
{
  static unsigned long lastUpdateTime = 0;
  const unsigned long updateInterval = 5000; // Update every 5 seconds

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

  unsigned long currentMillis = millis();


  if (currentMillis - lastUpdateTime >= updateInterval) {

    Serial.println(waterTemp());
    // Serial.println(airTemp());
    // Serial.println(UVlevel());
    // Serial.println(brightness());
    // Serial.println(waterLevel());

    // Send sensor data to the server
    sendSensorData("Water Temp", waterTemp());
    // sendSensorData("Air Temp", airTemp());
    // sendSensorData("UV Level", UVlevel());
    // sendSensorData("Brightness Level", brightness());
    // sendSensorData("Water Level", waterLevel());

    // Update the last update time
    lastUpdateTime = currentMillis;
  }

}