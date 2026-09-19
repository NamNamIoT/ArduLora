#ifndef __ARDULORA_MLX90614_H__
#define __ARDULORA_MLX90614_H__

#include <Arduino.h>
#include <Wire.h>

#define MLX90614_DEFAULT_ADDR 0x5A

#define MLX90614_RAW_TA       0x06  // Ambient temp register
#define MLX90614_RAW_TOBJ1    0x07  // Object 1 temp register

class ArduLora_MLX90614 {
  private:
    uint8_t _addr;
    TwoWire *_wire;

    float readTemp(uint8_t reg);

  public:
    ArduLora_MLX90614(uint8_t addr = MLX90614_DEFAULT_ADDR, TwoWire *wire = &Wire);

    bool begin();
    float readAmbientTempC();
    float readObjectTempC();
};

#endif
