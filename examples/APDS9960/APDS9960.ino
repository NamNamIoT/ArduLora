#include <ArduLora.h>
#include <Arduino.h>
#include <Wire.h>
#include <ArduLora_APDS9960.h>

ArduLora_APDS9960 apds(APDS9960_DEFAULT_ADDR, &Wire);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\r\n************ ArduLora APDS9960 Example ************");

  // Enable sensor power gate (PB5 Active LOW)
  pinMode(PB5, OUTPUT);
  digitalWrite(PB5, LOW);
  delay(100);

  Wire.begin();

  while (!apds.begin()) {
    Serial.println("APDS9960 sensor not detected! Retrying in 1s...");
    delay(1000);
  }
  Serial.println("APDS9960 initialized successfully.");

  apds.enableProximity(true);
  apds.enableGesture(true);
}

void loop() {
  uint8_t prox = apds.readProximity();
  Serial.print("Proximity: ");
  Serial.print(prox);

  if (apds.isGestureAvailable()) {
    apds9960_gesture_t gesture = apds.readGesture();
    switch (gesture) {
      case DIR_UP:
        Serial.println(" | Gesture: UP");
        break;
      case DIR_DOWN:
        Serial.println(" | Gesture: DOWN");
        break;
      case DIR_LEFT:
        Serial.println(" | Gesture: LEFT");
        break;
      case DIR_RIGHT:
        Serial.println(" | Gesture: RIGHT");
        break;
      default:
        Serial.println();
        break;
    }
  } else {
    Serial.println();
  }

  delay(200);
}
