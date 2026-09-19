#include "ArduLora_VL53L0X.h"

ArduLora_VL53L0X::ArduLora_VL53L0X(uint8_t addr, TwoWire *wire) : _addr(addr), _wire(wire), _stopVariable(0) {
}

uint8_t ArduLora_VL53L0X::read8(uint8_t reg) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(_addr, (uint8_t)1);
    return _wire->read();
}

uint16_t ArduLora_VL53L0X::read16(uint8_t reg) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(_addr, (uint8_t)2);
    uint8_t msb = _wire->read();
    uint8_t lsb = _wire->read();
    return ((uint16_t)msb << 8) | lsb;
}

void ArduLora_VL53L0X::write8(uint8_t reg, uint8_t val) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->write(val);
    _wire->endTransmission();
}

void ArduLora_VL53L0X::write16(uint8_t reg, uint16_t val) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->write((uint8_t)(val >> 8));
    _wire->write((uint8_t)(val & 0xFF));
    _wire->endTransmission();
}

bool ArduLora_VL53L0X::initSensor() {
    // Standard VL53L0X initialization sequence
    // Set 2.8V mode
    write8(0x89, read8(0x89) | 0x01);

    // Set I2C standard mode
    write8(0x88, 0x00);

    write8(0x80, 0x01);
    write8(0xFF, 0x01);
    write8(0x00, 0x00);
    _stopVariable = read8(0x91);
    write8(0x00, 0x01);
    write8(0xFF, 0x00);
    write8(0x80, 0x00);

    // Disable SIGNAL_RATE_MSRC and SIGNAL_RATE_PRE_RANGE limit checks
    write8(0x60, read8(0x60) | 0x12);

    // Set default signal rate limit (0.25 MCPS)
    write16(0x44, (uint16_t)(0.25f * (1 << 7)));

    write8(0x01, 0xFF);

    // Set SYSTEM_SEQUENCE_CONFIG
    write8(0x01, 0xE8);

    // Default tuning settings
    write8(0xFF, 0x01);
    write8(0x4E, 0x2C);
    write8(0x48, 0x00);
    write8(0x30, 0x20);
    write8(0xFF, 0x00);
    write8(0x30, 0x09);
    write8(0x54, 0x00);
    write8(0x31, 0x04);
    write8(0x32, 0x03);
    write8(0x40, 0x83);
    write8(0x46, 0x25);
    write8(0x60, 0x00);
    write8(0x27, 0x00);
    write8(0x50, 0x06);
    write8(0x51, 0x00);
    write8(0x52, 0x96);
    write8(0x56, 0x08);
    write8(0x57, 0x30);
    write8(0x61, 0x00);
    write8(0x62, 0x00);
    write8(0x64, 0x00);
    write8(0x65, 0x00);
    write8(0x66, 0xA0);
    write8(0xFF, 0x01);
    write8(0x22, 0x32);
    write8(0x47, 0x14);
    write8(0x49, 0xFF);
    write8(0x4A, 0x00);
    write8(0xFF, 0x00);
    write8(0x7A, 0x0A);
    write8(0x7B, 0x00);
    write8(0x78, 0x21);
    write8(0xFF, 0x01);
    write8(0x23, 0x34);
    write8(0x42, 0x00);
    write8(0x44, 0xFF);
    write8(0x45, 0x26);
    write8(0x46, 0x05);
    write8(0x40, 0x40);
    write8(0x0E, 0x06);
    write8(0x20, 0x1A);
    write8(0x43, 0x40);
    write8(0xFF, 0x00);
    write8(0x34, 0x03);
    write8(0x35, 0x44);
    write8(0xFF, 0x01);
    write8(0x31, 0x04);
    write8(0x4B, 0x09);
    write8(0x4C, 0x05);
    write8(0x4D, 0x04);
    write8(0xFF, 0x00);
    write8(0x44, 0x00);
    write8(0x45, 0x20);
    write8(0x47, 0x08);
    write8(0x48, 0x28);
    write8(0x67, 0x00);
    write8(0x70, 0x04);
    write8(0x71, 0x01);
    write8(0x72, 0xFE);
    write8(0x76, 0x00);
    write8(0x77, 0x00);
    write8(0xFF, 0x01);
    write8(0x0D, 0x01);
    write8(0xFF, 0x00);
    write8(0x80, 0x01);
    write8(0x01, 0xF8);
    write8(0xFF, 0x01);
    write8(0x8E, 0x01);
    write8(0x00, 0x01);
    write8(0xFF, 0x00);
    write8(0x80, 0x00);

    // Set interrupt config to new sample ready
    write8(0x0A, 0x04);
    write8(0x84, read8(0x84) & ~0x10); // active low
    write8(0x0B, 0x01);

    return true;
}

bool ArduLora_VL53L0X::begin() {
    _wire->beginTransmission(_addr);
    if (_wire->endTransmission() != 0) {
        return false;
    }

    // Check Model ID register (0xC0 should be 0xEE)
    uint8_t id = read8(0xC0);
    if (id != 0xEE) {
        return false;
    }

    return initSensor();
}

uint16_t ArduLora_VL53L0X::readRangeMillimeters() {
    write8(0x80, 0x01);
    write8(0xFF, 0x01);
    write8(0x00, 0x00);
    write8(0x91, _stopVariable);
    write8(0x00, 0x01);
    write8(0xFF, 0x00);
    write8(0x80, 0x00);

    // Start single shot measurement
    write8(0x00, 0x01);

    uint32_t start = millis();
    while (read8(0x00) & 0x01) {
        if (millis() - start > 200) return 8190; // Timeout
    }

    start = millis();
    while ((read8(0x13) & 0x07) == 0) {
        if (millis() - start > 200) return 8190; // Timeout
    }

    // Read 2-byte distance from RESULT_RANGE_STATUS + 10 (reg 0x1E)
    uint16_t range = read16(0x1E);

    // Clear interrupt
    write8(0x0B, 0x01);

    return range;
}
