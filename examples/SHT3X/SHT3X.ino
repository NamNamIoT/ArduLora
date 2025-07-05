#include <Arduino.h>  // Include the Arduino core library.
#include <Wire.h>  // Include the Wire library for I2C communication.
#include <ArtronShop_SHT3x.h>  // Include the SHT3x library.
ArtronShop_SHT3x sht3x(0x44, &Wire);  // ADDR: 0 => 0x44, ADDR: 1 => 0x45

void setup() {
Serial.begin(115200);  // Initialize serial communication at 115200 baud rate.
Serial.print("\r\n************ArduLora**************");  // Print a message indicating the start of the program.
//Enable power for external sensor
pinMode(PB5, OUTPUT);
digitalWrite(PB5, HIGH);
delay(100);  // Wait for 100 milliseconds.
Wire.begin();  // Initialize the I2C communication.
while (!sht3x.begin()) {  // Check if SHT3x sensor is detected.
  Serial.println("SHT3x not found !");  // Print a message if SHT3x sensor is not detected.
  delay(1000);  // Wait for 1 second before retrying.
}
}

void loop() {
if (sht3x.measure()) {  // Check if the measurement is successful.
  Serial.print("Temperature: ");  // Print a label indicating the temperature measurement.
  Serial.print(sht3x.temperature(), 1);  // Print the temperature value with one decimal place.
  Serial.print(" *C\tHumidity: ");  // Print a label indicating the humidity measurement.
  Serial.print(sht3x.humidity(), 1);  // Print the humidity value with one decimal place.
  Serial.print(" %RH");  // Print unit (% relative humidity).
  Serial.println();  // Print a newline character.
} else {
  Serial.println("SHT3x read error");  // Print a message if there is an error reading from the SHT3x sensor.
}
delay(1000);  // Wait for 1 second before the next measurement.
}
