#ifndef __ARDULORA_MPU6050_H__
#define __ARDULORA_MPU6050_H__

#include <Arduino.h>
#include <Wire.h>

#define MPU6050_DEFAULT_ADDR 0x68
#define MPU6050_ALT_ADDR     0x69

class ArduLora_MPU6050 {
  private:
    uint8_t _addr;
    TwoWire *_wire;

    int16_t _ax, _ay, _az;
    int16_t _gx, _gy, _gz;
    int16_t _rawTemp;

    uint8_t read8(uint8_t reg);
    void write8(uint8_t reg, uint8_t val);

  public:
    ArduLora_MPU6050(uint8_t addr = MPU6050_DEFAULT_ADDR, TwoWire *wire = &Wire);

    bool begin();
    bool update();

    // Accelerometer in 'g' (1g = 9.81 m/s^2)
    float accelX();
    float accelY();
    float accelZ();

    // Gyroscope in degrees per second (°/s)
    float gyroX();
    float gyroY();
    float gyroZ();

    // Internal chip temperature in °C
    float temperature();
};

#endif
