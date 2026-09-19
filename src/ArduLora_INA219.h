#ifndef __ARDULORA_INA219_H__
#define __ARDULORA_INA219_H__

#include <Arduino.h>
#include <Wire.h>

#define INA219_DEFAULT_ADDR 0x40

#define INA219_REG_CONFIG       0x00
#define INA219_REG_SHUNTVOLTAGE 0x01
#define INA219_REG_BUSVOLTAGE   0x02
#define INA219_REG_POWER        0x03
#define INA219_REG_CURRENT      0x04
#define INA219_REG_CALIBRATION  0x05

class ArduLora_INA219 {
  private:
    uint8_t _addr;
    TwoWire *_wire;
    uint32_t _calValue;
    float _currentLSB_mA;
    float _powerLSB_mW;

    int16_t read16(uint8_t reg);
    void write16(uint8_t reg, uint16_t val);

  public:
    ArduLora_INA219(uint8_t addr = INA219_DEFAULT_ADDR, TwoWire *wire = &Wire);

    bool begin();
    void setCalibration_32V_2A();
    float busVoltage_V();
    float shuntVoltage_mV();
    float current_mA();
    float power_mW();
};

#endif
