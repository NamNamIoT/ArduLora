#ifndef __ARDULORA_CCS811_H__
#define __ARDULORA_CCS811_H__

#include <Arduino.h>
#include <Wire.h>

#define CCS811_DEFAULT_ADDR 0x5A
#define CCS811_ALT_ADDR     0x5B

#define CCS811_REG_STATUS          0x00
#define CCS811_REG_MEAS_MODE       0x01
#define CCS811_REG_ALG_RESULT_DATA 0x02
#define CCS811_REG_HW_ID           0x20
#define CCS811_BOOTLOADER_APP_START 0xF4

class ArduLora_CCS811 {
  private:
    uint8_t _addr;
    TwoWire *_wire;
    int8_t _wakePin;

    uint16_t _eco2;
    uint16_t _tvoc;

    uint8_t read8(uint8_t reg);
    void write8(uint8_t reg, uint8_t val);

  public:
    ArduLora_CCS811(uint8_t addr = CCS811_DEFAULT_ADDR, int8_t wakePin = -1, TwoWire *wire = &Wire);

    bool begin();
    bool dataAvailable();
    bool read();

    uint16_t eCO2(); // in ppm (parts per million)
    uint16_t TVOC(); // in ppb (parts per billion)
};

#endif
