#include <ArduLora.h>
#include <Arduino.h>
#include <Wire.h>
#include <ArduLora_INA219.h>

ArduLora_INA219 ina219(INA219_DEFAULT_ADDR, &Wire);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\r\n************ ArduLora INA219 Example ************");

  // Enable sensor power gate (PB5 Active LOW)
  pinMode(PB5, OUTPUT);
  digitalWrite(PB5, LOW);
  delay(100);

  Wire.begin();

  while (!ina219.begin()) {
    Serial.println("INA219 sensor not found! Retrying in 1s...");
    delay(1000);
  }
  Serial.println("INA219 initialized successfully.");
}

void loop() {
  float bus_V = ina219.busVoltage_V();
  float shunt_mV = ina219.shuntVoltage_mV();
  float current_mA = ina219.current_mA();
  float power_mW = ina219.power_mW();

  Serial.print("Bus: ");
  Serial.print(bus_V);
  Serial.print(" V | Current: ");
  Serial.print(current_mA);
  Serial.print(" mA | Power: ");
  Serial.print(power_mW);
  Serial.print(" mW | Shunt: ");
  Serial.print(shunt_mV);
  Serial.println(" mV");

  delay(1000);
}
