#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 7 // Define the pin for the OneWire data

OneWire oneWire(ONE_WIRE_BUS); // Create a OneWire object
DallasTemperature sensors(&oneWire); // Create a DallasTemperature object

void setup() {
  Serial.begin(115200); // Initialize the serial communication
  sensors.begin(); // Initialize the DallasTemperature library
}

void loop() {
  sensors.requestTemperatures(); // Request temperature measurements from the sensor
  float temperature = sensors.getTempCByIndex(0); // Get the temperature measurement in Celsius
  //Serial.print("Temperature: "); // Print the temperature to the Serial Monitor
  Serial.println(temperature);
  //Serial.println(" °C");

  delay(500); // Wait for 1 second
}