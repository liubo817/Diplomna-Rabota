#include <math.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define ds18b20WaterPin 16
#define ds18b20Pin 17
#define uvPin 32
#define turbidityPin 33
#define waterLevelPin 35
#define brightnessPin 36
#define relayLamp 19
#define relayUV 18
#define relayHeater 5
#define waterLevelSensorPower 21


bool auto_mode = false;
const float minLogValue = 2.5;  
const float maxLogValue = 10.0;

const char* ssid = ""; //YourWiFiSSID
const char* password = ""; //YourWiFiPassword
const char* serverAddress = "http://192.168.100.10:5000/receive_data";
const int serverPort = 80;


WiFiServer server(serverPort);

OneWire oneWire(ds18b20WaterPin);	
DallasTemperature ds18b20_water(&oneWire);

OneWire oneWireAir(ds18b20Pin);	
DallasTemperature ds18b20(&oneWireAir);

void sendSensorData(const char* sensorType, float data, const char* measuringUnit) {
  
  HTTPClient http;
  
  if (WiFi.status() == WL_CONNECTED) // Check if WiFi is connected
  {
    http.begin(serverAddress); // Your Flask server endpoint
    http.addHeader("Content-Type", "application/json"); // Specify content type
    
    String jsonData = "{\"sensorType\":\"" + String(sensorType) + "\",\"sensorData\":" + String(data) + ",\"measuringUnit\":\"" + String(measuringUnit) + "\"}"; // Create JSON payload
    int httpResponseCode = http.POST(jsonData); // Send the POST request
    

    if (httpResponseCode > 0) // Check for errors
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("HTTP Error: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected. Skipping HTTP request.");
  }
}




float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float scaleToLogarithmic(int sensorReading)
{
  float scaledValue = map(sensorReading, 0, 4095, minLogValue * 1000, maxLogValue * 1000) / 1000.0;
  return log10(scaledValue);
}

int turbidity()
{
  int turbidityPercentage;
  int turbidityValue = analogRead(turbidityPin);
  float logarithmicTurbidity = scaleToLogarithmic(turbidityValue);
  turbidityPercentage = map(turbidityValue, 2350, 1750, 0, 100);
  Serial.println(turbidityValue);
  //return turbidityValue;
  return turbidityPercentage;
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
  float waterLevelPercentage;
  waterLevel = analogRead(waterLevelPin);
	digitalWrite(waterLevelSensorPower, HIGH);
	delay(10);
	digitalWrite(waterLevelSensorPower, LOW);
  waterLevelPercentage = map(waterLevel, 555, 1670, 0, 100);
  return waterLevel;
	return waterLevelPercentage;
}


float UVlevel()
{
  int uvLevel = analogRead(uvPin);
  float outputVoltage = uvLevel * (3.3 / 4095);
  float uvIntensity = mapfloat(outputVoltage, 0.99, 2.8, 0.0, 15.0); //Convert voltage to UV intensity
  return uvIntensity;
}

int brightness()
{
  int brightnessLevel = analogRead(brightnessPin);
  int brightnessLux;
  brightnessLux = map(brightnessLevel, 0, 4095, 10, 1000);
  return brightnessLux;
}



void setup(void)
{
  pinMode(waterLevelSensorPower, OUTPUT);
  pinMode(uvPin, INPUT);
  pinMode(waterLevelPin, INPUT);
  pinMode(relayLamp, OUTPUT);
  pinMode(relayUV, OUTPUT);
  pinMode(relayHeater, OUTPUT);

  Serial.begin(9600);
  WiFi.begin(ssid, password);
  ds18b20_water.begin();
  ds18b20.begin();

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
  server.begin(); //Start the server
  Serial.println("Server started");
}


void loop()
{
  static unsigned long lastUpdateTime = 0;
  const unsigned long updateInterval = 5000; // Update every 5 seconds

  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');
    Serial.println("Received request: " + request);

    if (request.indexOf("set_auto_mode") != -1) {
      if (request.indexOf("state=ON") != -1) {
        auto_mode = true;
        Serial.println("Auto Mode turned ON");
          
    
      } else if (request.indexOf("state=OFF") != -1) {
        auto_mode = false;
        Serial.println("Auto Mode turned OFF");
      }
    } else {
      if (request.indexOf("state=ON") != -1) {
        if (request.indexOf("device_type=LAMP") != -1) {
          digitalWrite(relayLamp, HIGH);
          auto_mode = false;
          Serial.println("Turning ON the LAMP");
        } else if (request.indexOf("device_type=UV_LAMP") != -1) {
          digitalWrite(relayUV, HIGH);
          auto_mode = false;         
          Serial.println("Turning ON the UV LAMP");
        } else if (request.indexOf("device_type=HEATER") != -1) {
          digitalWrite(relayHeater, HIGH);
          auto_mode = false;
          Serial.println("Turning ON the HEATER");
        }
      } else if (request.indexOf("state=OFF") != -1) {
        if (request.indexOf("device_type=LAMP") != -1) {
          digitalWrite(relayLamp, LOW);
          auto_mode = false;
          Serial.println("Turning OFF the LAMP");
        } else if (request.indexOf("device_type=UV_LAMP") != -1) {
          digitalWrite(relayUV, LOW);
          auto_mode = false;
          Serial.println("Turning OFF the UV LAMP");
        } else if (request.indexOf("device_type=HEATER") != -1) {
          digitalWrite(relayHeater, LOW);
          auto_mode = false;
          Serial.println("Turning OFF the HEATER");
        }
      }
    }
  }

    if(auto_mode == true)
    {
      if (waterTemp() < 24)
        {
          digitalWrite(relayHeater, HIGH);
        }
        else if (waterTemp() > 30)
        {
          digitalWrite(relayHeater, LOW);
        }

        if (brightness() < 70)
        {
          digitalWrite(relayLamp, HIGH);
        }
        else if (brightness() > 95)
        {
          digitalWrite(relayLamp, LOW);
        }

        if (UVlevel() <= 0.5)
        {
          digitalWrite(relayUV, HIGH);
        }
        else if (UVlevel() > 1.5)
        {
          digitalWrite(relayUV, LOW);
        }
    }


  unsigned long currentMillis = millis();


  if (currentMillis - lastUpdateTime >= updateInterval) {

    //Serial.println(waterTemp());
    //Serial.println(airTemp());
    //Serial.println(UVlevel());
    Serial.println(brightness());
    //Serial.println(waterLevel());
    //Serial.println(turbidity());
    //Send sensor data to the server
    //sendSensorData("Water Temp", waterTemp(), "Celsius");
    //sendSensorData("Turbidity", turbidity(), "Percentage");
    //sendSensorData("Air Temp", airTemp(), "Celsius");
    //sendSensorData("UV Level", UVlevel(), "UV intensity mW/cm2");
    sendSensorData("Brightness Level", brightness(), "Lux");
    //sendSensorData("Water Level", waterLevel(), "Celsius");
    lastUpdateTime = currentMillis;
  }

}