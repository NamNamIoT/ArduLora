#include <Arduino.h>  // Include the Arduino core library.
#include <Wire.h>  // Include the Wire library for I2C communication.
#include <ArtronShop_BH1750.h>  // Include the BH1750 library.

ArtronShop_BH1750 bh1750(0x23, &Wire); // Non Jump ADDR: 0x23, Jump ADDR: 0x5C

void setup() {
  Serial.begin(115200);  // Initialize serial communication at 115200 baud rate.
  Serial.print("\r\n************ArduLora**************");  // Print a message indicating the start of the program.
  //Enable power for external sensor
  pinMode(PB5, OUTPUT);
  digitalWrite(PB5, HIGH);
  Wire.begin();  // Initialize the I2C communication.
  while (!bh1750.begin()) {  // Check if BH1750 sensor is detected.
    Serial.println("BH1750 not found !");  // Print a message if BH1750 sensor is not detected.
    delay(1000);  // Wait for 1 second before retrying.
  }
}

void loop() {
  Serial.print("Light: ");  // Print a label indicating the light intensity measurement.
  Serial.print(bh1750.light());  // Print the light intensity value.
  Serial.print(" lx");  // Print unit (lux).
  Serial.println();  // Print a newline character.
  delay(1000);  // Wait for 1 second before the next measurement.
}
