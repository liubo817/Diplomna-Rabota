#include <Arduino.h>

int sensorPin = 34; //A0 FOR ARDUINO/ 36 FOR ESP

void setup()
{ 
  Serial.begin(9600);

}
void loop() {
  int sensorValue = analogRead(sensorPin);
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
  Serial.println("%");
}