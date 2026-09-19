#ifndef __ARDULORA_VL53L0X_H__
#define __ARDULORA_VL53L0X_H__

#include <Arduino.h>
#include <Wire.h>

#define VL53L0X_DEFAULT_ADDR 0x29

class ArduLora_VL53L0X {
  private:
    uint8_t _addr;
    TwoWire *_wire;
    uint8_t _stopVariable;

    uint8_t read8(uint8_t reg);
    uint16_t read16(uint8_t reg);
    void write8(uint8_t reg, uint8_t val);
    void write16(uint8_t reg, uint16_t val);

    bool initSensor();

  public:
    ArduLora_VL53L0X(uint8_t addr = VL53L0X_DEFAULT_ADDR, TwoWire *wire = &Wire);

    bool begin();
    uint16_t readRangeMillimeters();
};

#endif
