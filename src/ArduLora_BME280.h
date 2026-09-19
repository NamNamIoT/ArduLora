#ifndef __ARDULORA_BME280_H__
#define __ARDULORA_BME280_H__

#include <Arduino.h>
#include <Wire.h>

#define BME280_DEFAULT_ADDR 0x76
#define BME280_ALT_ADDR     0x77

class ArduLora_BME280 {
  private:
    uint8_t _addr;
    TwoWire *_wire;
    int32_t _t_fine;

    // Calibration data
    uint16_t dig_T1;
    int16_t  dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
    uint8_t  dig_H1, dig_H3;
    int16_t  dig_H2, dig_H4, dig_H5;
    int8_t   dig_H6;

    float _temperature;
    float _pressure;
    float _humidity;

    void readCalibrationData();
    uint8_t read8(uint8_t reg);
    void write8(uint8_t reg, uint8_t val);

  public:
    ArduLora_BME280(uint8_t addr = BME280_DEFAULT_ADDR, TwoWire *wire = &Wire);

    bool begin();
    bool measure();

    float temperature();  // °C
    float pressure();     // hPa
    float humidity();     // %RH
};

#endif
