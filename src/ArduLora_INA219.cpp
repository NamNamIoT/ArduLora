#include "ArduLora_INA219.h"

ArduLora_INA219::ArduLora_INA219(uint8_t addr, TwoWire *wire) : _addr(addr), _wire(wire) {
    _calValue = 4096;
    _currentLSB_mA = 0.1f;
    _powerLSB_mW = 2.0f;
}

int16_t ArduLora_INA219::read16(uint8_t reg) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    if (_wire->endTransmission(false) != 0) return 0;

    if (_wire->requestFrom(_addr, (uint8_t)2) != 2) return 0;

    uint8_t msb = _wire->read();
    uint8_t lsb = _wire->read();
    return (int16_t)(((uint16_t)msb << 8) | lsb);
}

void ArduLora_INA219::write16(uint8_t reg, uint16_t val) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->write((uint8_t)(val >> 8));
    _wire->write((uint8_t)(val & 0xFF));
    _wire->endTransmission();
}

void ArduLora_INA219::setCalibration_32V_2A() {
    // Cal = trunc(0.04096 / (current_lsb * r_shunt))
    // For Rshunt = 0.1 ohm, current_lsb = 0.0001A (0.1mA) -> Cal = 4096
    _calValue = 4096;
    _currentLSB_mA = 0.1f;
    _powerLSB_mW = 2.0f;

    write16(INA219_REG_CALIBRATION, (uint16_t)_calValue);

    // Config: 32V Bus range, 320mV shunt range (+-3.2A max), 12-bit averaging, continuous mode
    // 0x399F = B0011 1001 1001 1111
    uint16_t config = 0x399F;
    write16(INA219_REG_CONFIG, config);
}

bool ArduLora_INA219::begin() {
    _wire->beginTransmission(_addr);
    if (_wire->endTransmission() != 0) {
        return false;
    }
    setCalibration_32V_2A();
    return true;
}

float ArduLora_INA219::busVoltage_V() {
    int16_t raw = read16(INA219_REG_BUSVOLTAGE);
    // Shift right 3 bits, multiply by 4mV (0.004V)
    return (float)((raw >> 3) * 4) * 0.001f;
}

float ArduLora_INA219::shuntVoltage_mV() {
    int16_t raw = read16(INA219_REG_SHUNTVOLTAGE);
    return (float)raw * 0.01f;
}

float ArduLora_INA219::current_mA() {
    // Sometimes calibration register is cleared by power glitch, re-write if needed
    write16(INA219_REG_CALIBRATION, (uint16_t)_calValue);
    int16_t raw = read16(INA219_REG_CURRENT);
    return (float)raw * _currentLSB_mA;
}

float ArduLora_INA219::power_mW() {
    write16(INA219_REG_CALIBRATION, (uint16_t)_calValue);
    int16_t raw = read16(INA219_REG_POWER);
    return (float)raw * _powerLSB_mW;
}
