#include <Arduino.h>

#define sensorPower 21
#define sensorPin 35
#define led 25
int water_level;
int water_level_percent;

void setup() {
	// Set D7 as an OUTPUT
  pinMode(sensorPower, OUTPUT);
  pinMode(sensorPin, INPUT);
  pinMode(led, OUTPUT);
	
	// Set to LOW so no power flows through the sensor
	digitalWrite(sensorPower, LOW);
	
	Serial.begin(9600);
}

int readSensor(int val) {
	digitalWrite(sensorPower, HIGH);	// Turn the sensor ON
	val = analogRead(sensorPin);		// Read the analog value form sensor
	digitalWrite(sensorPower, LOW);		// Turn the sensor OFF
	return val;							// send current reading
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int sensor_to_percent(int level)
{
  int sensor_data = map(level, 0, 850, 0, 100);
  return sensor_data;
}

void loop() {
  float voltage = readSensor(water_level) * (3.3 / 4095.0);
  int percent = map(analogRead(sensorPin), 750, 1900, 0, 100);
  water_level_percent = mapfloat(voltage, 0, 0.7, 0, 100);
  //Serial.print("Water level percentage: ");
  Serial.println(readSensor(water_level));
  //Serial.println(percent);
	delay(1000);
}

