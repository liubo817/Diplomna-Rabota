#include <Arduino.h>

#define sensorPower 21
#define sensorPin 2
#define led 25
int water_level;
int water_level_percent;

void setup() {
	// Set D7 as an OUTPUT
	pinMode(sensorPower, OUTPUT);
  pinMode(led, OUTPUT);
	
	// Set to LOW so no power flows through the sensor
	digitalWrite(sensorPower, LOW);
	
	Serial.begin(9600);
}

int readSensor(int val) {
	digitalWrite(sensorPower, HIGH);	// Turn the sensor ON
	delay(10);							// wait 10 milliseconds
	val = analogRead(sensorPin);		// Read the analog value form sensor
	digitalWrite(sensorPower, LOW);		// Turn the sensor OFF
	return val;							// send current reading
}

int sensor_to_percent(int level)
{
  int sensor_data = map(level, 0, 1400, 0, 100);
  return sensor_data;
}

void loop() {
	//get the reading from the function below and print it
  water_level_percent = sensor_to_percent(readSensor(water_level));
  Serial.print("Water level percentage: ");
  Serial.println(water_level_percent);
	if (water_level_percent <= 85)
  {
    digitalWrite(led, HIGH);
  }else{
    digitalWrite(led, LOW);
  }

	
	delay(1000);
}

//This is a function used to get the reading
