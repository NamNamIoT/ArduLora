#include "ArduLora_BME280.h"

ArduLora_BME280::ArduLora_BME280(uint8_t addr, TwoWire *wire) : _addr(addr), _wire(wire) {
    _temperature = -99.0f;
    _pressure = -99.0f;
    _humidity = -99.0f;
    _t_fine = 0;
}

uint8_t ArduLora_BME280::read8(uint8_t reg) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(_addr, (uint8_t)1);
    return _wire->read();
}

void ArduLora_BME280::write8(uint8_t reg, uint8_t val) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->write(val);
    _wire->endTransmission();
}

void ArduLora_BME280::readCalibrationData() {
    _wire->beginTransmission(_addr);
    _wire->write(0x88);
    _wire->endTransmission(false);
    _wire->requestFrom(_addr, (uint8_t)24);

    uint8_t b[24];
    for (int i = 0; i < 24; i++) {
        b[i] = _wire->read();
    }

    dig_T1 = (uint16_t)b[0] | ((uint16_t)b[1] << 8);
    dig_T2 = (int16_t)b[2] | ((int16_t)b[3] << 8);
    dig_T3 = (int16_t)b[4] | ((int16_t)b[5] << 8);

    dig_P1 = (uint16_t)b[6] | ((uint16_t)b[7] << 8);
    dig_P2 = (int16_t)b[8] | ((int16_t)b[9] << 8);
    dig_P3 = (int16_t)b[10] | ((int16_t)b[11] << 8);
    dig_P4 = (int16_t)b[12] | ((int16_t)b[13] << 8);
    dig_P5 = (int16_t)b[14] | ((int16_t)b[15] << 8);
    dig_P6 = (int16_t)b[16] | ((int16_t)b[17] << 8);
    dig_P7 = (int16_t)b[18] | ((int16_t)b[19] << 8);
    dig_P8 = (int16_t)b[20] | ((int16_t)b[21] << 8);
    dig_P9 = (int16_t)b[22] | ((int16_t)b[23] << 8);

    dig_H1 = read8(0xA1);

    _wire->beginTransmission(_addr);
    _wire->write(0xE1);
    _wire->endTransmission(false);
    _wire->requestFrom(_addr, (uint8_t)7);

    uint8_t h[7];
    for (int i = 0; i < 7; i++) {
        h[i] = _wire->read();
    }

    dig_H2 = (int16_t)h[0] | ((int16_t)h[1] << 8);
    dig_H3 = h[2];
    dig_H4 = ((int16_t)h[3] << 4) | (h[4] & 0x0F);
    dig_H5 = ((int16_t)h[5] << 4) | (h[4] >> 4);
    dig_H6 = (int8_t)h[6];
}

bool ArduLora_BME280::begin() {
    uint8_t chip_id = read8(0xD0);
    if (chip_id != 0x60 && chip_id != 0x58) { // 0x60 = BME280, 0x58 = BMP280
        return false;
    }

    readCalibrationData();

    // Humidity oversampling x1
    write8(0xF2, 0x01);
    // Config: standby 500ms, filter 16
    write8(0xF5, 0xA0);
    // ctrl_meas: temp oversampling x1, press oversampling x1, normal mode (0b00100111 = 0x27)
    write8(0xF4, 0x27);

    delay(50);
    return true;
}

bool ArduLora_BME280::measure() {
    _wire->beginTransmission(_addr);
    _wire->write(0xF7);
    if (_wire->endTransmission(false) != 0) return false;

    if (_wire->requestFrom(_addr, (uint8_t)8) != 8) return false;

    uint8_t data[8];
    for (int i = 0; i < 8; i++) {
        data[i] = _wire->read();
    }

    int32_t adc_P = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | ((uint32_t)data[2] >> 4);
    int32_t adc_T = ((uint32_t)data[3] << 12) | ((uint32_t)data[4] << 4) | ((uint32_t)data[5] >> 4);
    int32_t adc_H = ((uint32_t)data[6] << 8) | (uint32_t)data[7];

    // Temperature compensation
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    _t_fine = var1 + var2;
    _temperature = (_t_fine * 5 + 128) >> 8;
    _temperature /= 100.0f;

    // Pressure compensation
    int64_t p_var1 = ((int64_t)_t_fine) - 128000;
    int64_t p_var2 = p_var1 * p_var1 * (int64_t)dig_P6;
    p_var2 = p_var2 + ((p_var1 * (int64_t)dig_P5) << 17);
    p_var2 = p_var2 + (((int64_t)dig_P4) << 35);
    p_var1 = ((p_var1 * p_var1 * (int64_t)dig_P3) >> 8) + ((p_var1 * (int64_t)dig_P2) << 12);
    p_var1 = (((((int64_t)1) << 47) + p_var1)) * ((int64_t)dig_P1) >> 33;

    if (p_var1 == 0) {
        _pressure = 0.0f;
    } else {
        int64_t p = 1048576 - adc_P;
        p = (((p << 31) - p_var2) * 3125) / p_var1;
        p_var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
        p_var2 = (((int64_t)dig_P8) * p) >> 19;
        p = ((p + p_var1 + p_var2) >> 8) + (((int64_t)dig_P7) << 4);
        _pressure = (float)p / 256.0f / 100.0f; // in hPa
    }

    // Humidity compensation
    int32_t h_var = (_t_fine - ((int32_t)76800));
    h_var = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * h_var)) +
               ((int32_t)16384)) >> 15) * (((((((h_var * ((int32_t)dig_H6)) >> 10) *
               (((h_var * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
               ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
    h_var = (h_var - (((((h_var >> 15) * (h_var >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
    h_var = (h_var < 0) ? 0 : h_var;
    h_var = (h_var > 419430400) ? 419430400 : h_var;
    _humidity = (float)(h_var >> 12) / 1024.0f;

    return true;
}

float ArduLora_BME280::temperature() {
    return _temperature;
}

float ArduLora_BME280::pressure() {
    return _pressure;
}

float ArduLora_BME280::humidity() {
    return _humidity;
}
