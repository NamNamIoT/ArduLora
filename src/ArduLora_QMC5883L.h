#ifndef __ARDULORA_QMC5883L_H__
#define __ARDULORA_QMC5883L_H__

#include <Arduino.h>
#include <Wire.h>

#define QMC5883L_DEFAULT_ADDR 0x0D

class ArduLora_QMC5883L {
  private:
    uint8_t _addr;
    TwoWire *_wire;

    int16_t _x, _y, _z;

    void write8(uint8_t reg, uint8_t val);
    uint8_t read8(uint8_t reg);

  public:
    ArduLora_QMC5883L(uint8_t addr = QMC5883L_DEFAULT_ADDR, TwoWire *wire = &Wire);

    bool begin();
    bool read();

    int16_t rawX();
    int16_t rawY();
    int16_t rawZ();

    float heading(); // Azimuth in degrees (0 - 360°)
};

#endif
