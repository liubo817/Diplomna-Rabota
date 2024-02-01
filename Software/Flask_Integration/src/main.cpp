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

#define relay1 19


int water_level;
int water_level_percent;


const char* ssid = ""; //YourWiFiSSID
const char* password = ""; //YourWiFiPassword
const char* serverAddress = "http://:5000/receive_data";

OneWire oneWire(ds18b20WaterPin);	
DallasTemperature ds18b20_water(&oneWire);

OneWire oneWireAir(ds18b20Pin);	
DallasTemperature ds18b20(&oneWireAir);

void sendSensorData(const char* sensorType, float data) {
  HTTPClient http;
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
  pinMode(relay1, OUTPUT);
  ds18b20_water.begin();
  ds18b20.begin();
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
  //millis
  Serial.println(waterTemp());
  //Serial.println(airTemp());
  //Serial.println(UVlevel());
  //Serial.println(brightness());
  //Serial.println(waterLevel());

  if(waterTemp() > 27)
  {
    digitalWrite(relay1, 1);
  }else{
    digitalWrite(relay1, 0);
  }
  sendSensorData("Water Temp", waterTemp());
  //sendSensorData("Air Temp", airTemp());
  //sendSensorData(turbidity());
  //sendSensorData("Water Level", waterLevel());
  //sendSensorData("UV Level", UVlevel());
  //sendSensorData("Brightness Level", brightness());

  delay(5000);
}