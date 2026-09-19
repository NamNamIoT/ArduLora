#include "ArduLora_QMC5883L.h"
#include <math.h>

ArduLora_QMC5883L::ArduLora_QMC5883L(uint8_t addr, TwoWire *wire) : _addr(addr), _wire(wire) {
    _x = _y = _z = 0;
}

void ArduLora_QMC5883L::write8(uint8_t reg, uint8_t val) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->write(val);
    _wire->endTransmission();
}

uint8_t ArduLora_QMC5883L::read8(uint8_t reg) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(_addr, (uint8_t)1);
    return _wire->read();
}

bool ArduLora_QMC5883L::begin() {
    _wire->beginTransmission(_addr);
    if (_wire->endTransmission() != 0) {
        return false;
    }

    // Soft reset (reg 0x0A, bit 7 = 1)
    write8(0x0A, 0x80);
    delay(10);

    // Set/Reset period register (reg 0x0B = 0x01 recommended by datasheet)
    write8(0x0B, 0x01);

    // Control Register 1 (0x09):
    // Mode: Continuous (0x01)
    // Output Data Rate (ODR): 100Hz (0x08)
    // Full Scale (RNG): 8 Gauss (0x10)
    // Over Sample Ratio (OSR): 512 (0x00)
    // 0x01 | 0x08 | 0x10 = 0x1D
    write8(0x09, 0x1D);

    return true;
}

bool ArduLora_QMC5883L::read() {
    // Check status register (0x06): bit 0 is DRDY (Data Ready)
    uint8_t status = read8(0x06);
    if (!(status & 0x01)) {
        // Data not ready yet
        return false;
    }

    _wire->beginTransmission(_addr);
    _wire->write(0x00);
    if (_wire->endTransmission(false) != 0) return false;

    if (_wire->requestFrom(_addr, (uint8_t)6) != 6) return false;

    uint8_t x_lsb = _wire->read();
    uint8_t x_msb = _wire->read();
    uint8_t y_lsb = _wire->read();
    uint8_t y_msb = _wire->read();
    uint8_t z_lsb = _wire->read();
    uint8_t z_msb = _wire->read();

    _x = (int16_t)(((uint16_t)x_msb << 8) | x_lsb);
    _y = (int16_t)(((uint16_t)y_msb << 8) | y_lsb);
    _z = (int16_t)(((uint16_t)z_msb << 8) | z_lsb);

    return true;
}

int16_t ArduLora_QMC5883L::rawX() {
    return _x;
}

int16_t ArduLora_QMC5883L::rawY() {
    return _y;
}

int16_t ArduLora_QMC5883L::rawZ() {
    return _z;
}

float ArduLora_QMC5883L::heading() {
    float angle = atan2((float)_y, (float)_x) * 180.0f / (float)M_PI;
    if (angle < 0) {
        angle += 360.0f;
    }
    return angle;
}
