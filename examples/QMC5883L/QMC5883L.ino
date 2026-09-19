#include <ArduLora.h>
#include <Arduino.h>
#include <Wire.h>
#include <ArduLora_QMC5883L.h>

ArduLora_QMC5883L compass(QMC5883L_DEFAULT_ADDR, &Wire);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\r\n************ ArduLora QMC5883L Example ************");

  // Enable sensor power gate (PB5 Active LOW)
  pinMode(PB5, OUTPUT);
  digitalWrite(PB5, LOW);
  delay(100);

  Wire.begin();

  while (!compass.begin()) {
    Serial.println("QMC5883L compass not detected! Retrying in 1s...");
    delay(1000);
  }
  Serial.println("QMC5883L initialized successfully.");
}

void loop() {
  if (compass.read()) {
    Serial.print("Raw: X=");
    Serial.print(compass.rawX());
    Serial.print(" Y=");
    Serial.print(compass.rawY());
    Serial.print(" Z=");
    Serial.print(compass.rawZ());

    Serial.print(" | Heading: ");
    Serial.print(compass.heading(), 1);
    Serial.println(" deg");
  }

  delay(200);
}
