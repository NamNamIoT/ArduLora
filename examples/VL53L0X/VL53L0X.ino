#include <ArduLora.h>
#include <Arduino.h>
#include <Wire.h>
#include <ArduLora_VL53L0X.h>

ArduLora_VL53L0X tof(VL53L0X_DEFAULT_ADDR, &Wire);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\r\n************ ArduLora VL53L0X Example ************");

  // Enable sensor power gate (PB5 Active LOW)
  pinMode(PB5, OUTPUT);
  digitalWrite(PB5, LOW);
  delay(100);

  Wire.begin();

  while (!tof.begin()) {
    Serial.println("VL53L0X not detected! Retrying in 1s...");
    delay(1000);
  }
  Serial.println("VL53L0X initialized successfully.");
}

void loop() {
  uint16_t dist = tof.readRangeMillimeters();

  Serial.print("Distance: ");
  if (dist >= 8000) {
    Serial.println("Out of range / Error");
  } else {
    Serial.print(dist);
    Serial.print(" mm (");
    Serial.print((float)dist / 10.0f, 1);
    Serial.println(" cm)");
  }

  delay(200);
}
