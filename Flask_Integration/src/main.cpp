#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Data wire is plugged into digital pin 2 on the Arduino
#define ds18b20WaterPin 4
#define uvPin 32
#define uvRefPin 39
#define turbidityPin 33
#define waterLevelSensorPower 21
//#define waterLevelPin 2
#define brightnessPin 36


int water_level;
int water_level_percent;


const char* ssid = "A1_A2FAE9"; //YourWiFiSSID
const char* password = "13605f52"; //YourWiFiPassword
const char* serverAddress = "http://192.168.1.2:5000/receive_data";

OneWire oneWire(ds18b20WaterPin);	
DallasTemperature ds18b20_water(&oneWire);

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



float waterLevel()
{
  int waterLevel;
  int sensor_data;
  float waterLevelPercentage;
	digitalWrite(waterLevelSensorPower, HIGH);
	delay(10);
	//waterLevel = analogRead(waterLevelPin);
	digitalWrite(waterLevelSensorPower, LOW);
  //waterLevelPercentage = map(waterLevel, 0, 1400, 0, 100);
	//return waterLevelPercentage;
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
  ds18b20_water.begin();
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}


void loop(void)
{
  Serial.println(waterTemp());
  Serial.println(UVlevel());
  Serial.println(brightness());

  sendSensorData(waterTemp());
  //sendSensorData(turbidity());
  //sendSensorData(waterLevel());
  sendSensorData(UVlevel());
  sendSensorData(brightness());

  delay(5000);
}
