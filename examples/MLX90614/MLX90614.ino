#include <ArduLora.h>
#include <Arduino.h>
#include <Wire.h>
#include <ArduLora_MLX90614.h>

ArduLora_MLX90614 mlx(MLX90614_DEFAULT_ADDR, &Wire);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\r\n************ ArduLora MLX90614 Example ************");

  // Enable sensor power gate (PB5 Active LOW)
  pinMode(PB5, OUTPUT);
  digitalWrite(PB5, LOW);
  delay(100);

  Wire.begin();

  while (!mlx.begin()) {
    Serial.println("MLX90614 sensor not detected! Retrying in 1s...");
    delay(1000);
  }
  Serial.println("MLX90614 initialized successfully.");
}

void loop() {
  float ambient = mlx.readAmbientTempC();
  float object = mlx.readObjectTempC();

  Serial.print("Ambient: ");
  Serial.print(ambient);
  Serial.print(" *C | Object: ");
  Serial.print(object);
  Serial.println(" *C");

  delay(1000);
}
