#include <ArduLora.h>
#include <Arduino.h>
#include <Wire.h>
#include <ArduLora_CCS811.h>

// Note: If WAKE pin is tied to GND on module, pass -1.
// If connected to a GPIO pin, specify the pin number.
ArduLora_CCS811 ccs811(CCS811_DEFAULT_ADDR, -1, &Wire);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\r\n************ ArduLora CCS811 Example ************");

  // Enable sensor power gate (PB5 Active LOW)
  pinMode(PB5, OUTPUT);
  digitalWrite(PB5, LOW);
  delay(100);

  Wire.begin();

  while (!ccs811.begin()) {
    Serial.println("CCS811 sensor not detected! Retrying in 1s...");
    delay(1000);
  }
  Serial.println("CCS811 initialized successfully. Preheating sensor...");
}

void loop() {
  if (ccs811.dataAvailable()) {
    if (ccs811.read()) {
      Serial.print("eCO2: ");
      Serial.print(ccs811.eCO2());
      Serial.print(" ppm | TVOC: ");
      Serial.print(ccs811.TVOC());
      Serial.println(" ppb");
    }
  } else {
    Serial.println("Waiting for CCS811 data...");
  }

  delay(1000);
}
