#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define turPin 34
#define sensorPower 21
#define waterLevelPin 2
#define led 25
#define ONE_WIRE_BUS 4

int water_level;
int water_level_percent;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup()
{ 
  Serial.begin(9600);
  sensors.begin();
  pinMode(sensorPower, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(sensorPower, LOW);


}

int readSensor(int val) {
	digitalWrite(sensorPower, HIGH);	// Turn the sensor ON
	delay(10);							// wait 10 milliseconds
	val = analogRead(waterLevelPin);		// Read the analog value form sensor
	digitalWrite(sensorPower, LOW);		// Turn the sensor OFF
	return val;							// send current reading
}

int sensor_to_percent(int level)
{
  int sensor_data = map(level, 0, 2000, 0, 100);
  return sensor_data;
}

void loop() {
  int sensorValue = analogRead(turPin);
  int clean = map(sensorValue, 0, 4095, 100, 0);
  //Serial.println(sensorValue);
  sensors.requestTemperatures(); 
  float temp = sensors.getTempCByIndex(0);
  water_level_percent = sensor_to_percent(readSensor(waterLevelPin));
  Serial.print("Water level percentage: ");
  Serial.println(water_level_percent);
	if (water_level_percent <= 85 && temp >= 25)
  {
    analogWrite(led, 100);
  }else{
    analogWrite(led, 0);
  }

	
	delay(1000);

  //print the temperature in Celsius
  Serial.print("Temperature: ");
  Serial.print(sensors.getTempCByIndex(0));
  Serial.print("C");
  Serial.println();

  
  delay(300);
  Serial.print("Cleanliness: ");
  Serial.print(clean);
  Serial.println("%");
}