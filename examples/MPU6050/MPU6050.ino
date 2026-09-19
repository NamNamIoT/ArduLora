#include <ArduLora.h>
#include <Arduino.h>
#include <Wire.h>
#include <ArduLora_MPU6050.h>

ArduLora_MPU6050 mpu(MPU6050_DEFAULT_ADDR, &Wire);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\r\n************ ArduLora MPU6050 Example ************");

  // Enable sensor power gate (PB5 Active LOW)
  pinMode(PB5, OUTPUT);
  digitalWrite(PB5, LOW);
  delay(100);

  Wire.begin();

  while (!mpu.begin()) {
    Serial.println("MPU6050 not detected! Retrying in 1s...");
    delay(1000);
  }
  Serial.println("MPU6050 initialized successfully.");
}

void loop() {
  if (mpu.update()) {
    Serial.print("Accel (g): X=");
    Serial.print(mpu.accelX(), 2);
    Serial.print(" Y=");
    Serial.print(mpu.accelY(), 2);
    Serial.print(" Z=");
    Serial.print(mpu.accelZ(), 2);

    Serial.print(" | Gyro (deg/s): X=");
    Serial.print(mpu.gyroX(), 1);
    Serial.print(" Y=");
    Serial.print(mpu.gyroY(), 1);
    Serial.print(" Z=");
    Serial.print(mpu.gyroZ(), 1);

    Serial.print(" | Temp: ");
    Serial.print(mpu.temperature(), 1);
    Serial.println(" *C");
  } else {
    Serial.println("Failed to read MPU6050 data!");
  }

  delay(200);
}
