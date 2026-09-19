#include "ArduLora_MPU6050.h"

ArduLora_MPU6050::ArduLora_MPU6050(uint8_t addr, TwoWire *wire) : _addr(addr), _wire(wire) {
    _ax = _ay = _az = 0;
    _gx = _gy = _gz = 0;
    _rawTemp = 0;
}

uint8_t ArduLora_MPU6050::read8(uint8_t reg) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(_addr, (uint8_t)1);
    return _wire->read();
}

void ArduLora_MPU6050::write8(uint8_t reg, uint8_t val) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->write(val);
    _wire->endTransmission();
}

bool ArduLora_MPU6050::begin() {
    // Check WHO_AM_I register (0x75)
    uint8_t who = read8(0x75);
    if (who != 0x68) {
        return false;
    }

    // Wake up MPU6050: write 0 to PWR_MGMT_1 (0x6B)
    write8(0x6B, 0x00);
    delay(10);

    // Accel config +-2g (AFS_SEL = 0)
    write8(0x1C, 0x00);
    // Gyro config +-250 deg/s (FS_SEL = 0)
    write8(0x1B, 0x00);

    return true;
}

bool ArduLora_MPU6050::update() {
    _wire->beginTransmission(_addr);
    _wire->write(0x3B); // ACCEL_XOUT_H
    if (_wire->endTransmission(false) != 0) return false;

    if (_wire->requestFrom(_addr, (uint8_t)14) != 14) return false;

    _ax = ((int16_t)_wire->read() << 8) | _wire->read();
    _ay = ((int16_t)_wire->read() << 8) | _wire->read();
    _az = ((int16_t)_wire->read() << 8) | _wire->read();
    _rawTemp = ((int16_t)_wire->read() << 8) | _wire->read();
    _gx = ((int16_t)_wire->read() << 8) | _wire->read();
    _gy = ((int16_t)_wire->read() << 8) | _wire->read();
    _gz = ((int16_t)_wire->read() << 8) | _wire->read();

    return true;
}

float ArduLora_MPU6050::accelX() {
    return (float)_ax / 16384.0f;
}

float ArduLora_MPU6050::accelY() {
    return (float)_ay / 16384.0f;
}

float ArduLora_MPU6050::accelZ() {
    return (float)_az / 16384.0f;
}

float ArduLora_MPU6050::gyroX() {
    return (float)_gx / 131.0f;
}

float ArduLora_MPU6050::gyroY() {
    return (float)_gy / 131.0f;
}

float ArduLora_MPU6050::gyroZ() {
    return (float)_gz / 131.0f;
}

float ArduLora_MPU6050::temperature() {
    return ((float)_rawTemp / 340.0f) + 36.53f;
}
