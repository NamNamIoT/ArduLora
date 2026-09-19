#include <ArduLora.h>
#include <Arduino.h>
#include <Wire.h>
#include <ArduLora_BME280.h>

ArduLora_BME280 bme280(BME280_DEFAULT_ADDR, &Wire); // 0x76 or 0x77

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\r\n************ ArduLora BME280 Example ************");

  // Enable power for external sensor (PB5 Active LOW)
  pinMode(PB5, OUTPUT);
  digitalWrite(PB5, LOW);
  delay(100);

  Wire.begin();

  while (!bme280.begin()) {
    Serial.println("BME280 sensor not found! Retrying in 1s...");
    delay(1000);
  }
  Serial.println("BME280 initialized successfully.");
}

void loop() {
  if (bme280.measure()) {
    Serial.print("Temperature: ");
    Serial.print(bme280.temperature());
    Serial.print(" *C | Pressure: ");
    Serial.print(bme280.pressure());
    Serial.print(" hPa | Humidity: ");
    Serial.print(bme280.humidity());
    Serial.println(" %");
  } else {
    Serial.println("Failed to read from BME280!");
  }

  delay(2000);
}
