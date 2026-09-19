#include "ArduLora_CCS811.h"

ArduLora_CCS811::ArduLora_CCS811(uint8_t addr, int8_t wakePin, TwoWire *wire) 
    : _addr(addr), _wakePin(wakePin), _wire(wire) {
    _eco2 = 400;
    _tvoc = 0;
}

uint8_t ArduLora_CCS811::read8(uint8_t reg) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(_addr, (uint8_t)1);
    return _wire->read();
}

void ArduLora_CCS811::write8(uint8_t reg, uint8_t val) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->write(val);
    _wire->endTransmission();
}

bool ArduLora_CCS811::begin() {
    if (_wakePin >= 0) {
        pinMode(_wakePin, OUTPUT);
        digitalWrite(_wakePin, LOW); // Pull WAKE LOW to enable communication
        delay(10);
    }

    _wire->beginTransmission(_addr);
    if (_wire->endTransmission() != 0) {
        return false;
    }

    // Verify Hardware ID (0x20 should be 0x81)
    uint8_t hw_id = read8(CCS811_REG_HW_ID);
    if (hw_id != 0x81) {
        return false;
    }

    // Check status register
    uint8_t status = read8(CCS811_REG_STATUS);

    // If in boot mode (bit 7 == 0), transition to app mode if APP_VALID (bit 4 == 1)
    if ((status & 0x80) == 0) {
        if (!(status & 0x10)) {
            return false; // No valid application firmware
        }

        // Send APP_START command
        _wire->beginTransmission(_addr);
        _wire->write(CCS811_BOOTLOADER_APP_START);
        _wire->endTransmission();
        delay(20);
    }

    // Configure Measurement Mode: Mode 1 (Drive mode 1 = measurement every 1 sec)
    // 0x10 = 0b00010000
    write8(CCS811_REG_MEAS_MODE, 0x10);

    return true;
}

bool ArduLora_CCS811::dataAvailable() {
    uint8_t status = read8(CCS811_REG_STATUS);
    return (status & 0x08) != 0; // Bit 3 is DATA_READY
}

bool ArduLora_CCS811::read() {
    if (!dataAvailable()) {
        return false;
    }

    _wire->beginTransmission(_addr);
    _wire->write(CCS811_REG_ALG_RESULT_DATA);
    if (_wire->endTransmission(false) != 0) return false;

    if (_wire->requestFrom(_addr, (uint8_t)4) != 4) return false;

    uint8_t eco2_msb = _wire->read();
    uint8_t eco2_lsb = _wire->read();
    uint8_t tvoc_msb = _wire->read();
    uint8_t tvoc_lsb = _wire->read();

    _eco2 = ((uint16_t)eco2_msb << 8) | eco2_lsb;
    _tvoc = ((uint16_t)tvoc_msb << 8) | tvoc_lsb;

    return true;
}

uint16_t ArduLora_CCS811::eCO2() {
    return _eco2;
}

uint16_t ArduLora_CCS811::TVOC() {
    return _tvoc;
}
