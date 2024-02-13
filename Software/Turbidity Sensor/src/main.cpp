#include <Arduino.h>
#include "CQRobotTDS.h"


int sensorPin = 33;

CQRobotTDS tds(sensorPin);
unsigned long timeout = 0;

void setup()
{ 
  Serial.begin(9600);

}
void loop() {
  float temp = 25.0; // read temprature from a real sensor
	float tdsValue = tds.update(temp);

	if (timeout<millis())
	{
		Serial.print("TDS value: ");
		Serial.print(tdsValue, 0);
		Serial.println(" ppm");
		timeout=millis() + 1000;
	}

  /*int sensorValue = analogRead(sensorPin);
  //Serial.println(sensorValue);
  int turbidity = map(sensorValue, 0, 4095, 0, 100);
  int clean = map(turbidity, 0, 100, 100, 0);
  delay(300);
  // Serial.println("Turbidity: ");
  // Serial.print(turbidity);
  // Serial.print("%");
  // delay(500);
  Serial.print("Cleanliness: ");
  Serial.print(clean);
  Serial.println("%");*/
}