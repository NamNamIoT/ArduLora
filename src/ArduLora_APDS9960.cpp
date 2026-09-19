#include "ArduLora_APDS9960.h"

ArduLora_APDS9960::ArduLora_APDS9960(uint8_t addr, TwoWire *wire) : _addr(addr), _wire(wire) {
}

uint8_t ArduLora_APDS9960::read8(uint8_t reg) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(_addr, (uint8_t)1);
    return _wire->read();
}

uint16_t ArduLora_APDS9960::read16(uint8_t reg) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(_addr, (uint8_t)2);
    uint8_t lsb = _wire->read();
    uint8_t msb = _wire->read();
    return ((uint16_t)msb << 8) | lsb;
}

void ArduLora_APDS9960::write8(uint8_t reg, uint8_t val) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    _wire->write(val);
    _wire->endTransmission();
}

bool ArduLora_APDS9960::begin() {
    _wire->beginTransmission(_addr);
    if (_wire->endTransmission() != 0) {
        return false;
    }

    uint8_t id = read8(0x92);
    if (id != 0xAB && id != 0x9C && id != 0xA8) {
        return false;
    }

    // Disable everything first
    write8(0x80, 0x00);

    // Set ADC integration time: 101ms (0xD5)
    write8(0x81, 0xD5);
    // Set Wait time: 2.78ms (0xFF)
    write8(0x83, 0xFF);
    // Control: LED drive 100mA, PGAIN 4x, AGAIN 4x
    write8(0x8F, 0x25);
    // Pulse config: 8 pulses of 16us
    write8(0x8E, 0x87);

    // Turn Power ON (PON bit 0)
    write8(0x80, 0x01);
    delay(10);

    return true;
}

void ArduLora_APDS9960::enableProximity(bool enable) {
    uint8_t en = read8(0x80);
    if (enable) {
        en |= 0x05; // PON + PEN
    } else {
        en &= ~0x04;
    }
    write8(0x80, en);
}

uint8_t ArduLora_APDS9960::readProximity() {
    return read8(0x9C);
}

void ArduLora_APDS9960::enableColor(bool enable) {
    uint8_t en = read8(0x80);
    if (enable) {
        en |= 0x03; // PON + AEN
    } else {
        en &= ~0x02;
    }
    write8(0x80, en);
}

bool ArduLora_APDS9960::readColor(uint16_t &r, uint16_t &g, uint16_t &b, uint16_t &c) {
    // Check status: AVALID bit 0 of 0x93
    uint8_t status = read8(0x93);
    if (!(status & 0x01)) {
        return false;
    }

    c = read16(0x94);
    r = read16(0x96);
    g = read16(0x98);
    b = read16(0x9A);
    return true;
}

void ArduLora_APDS9960::enableGesture(bool enable) {
    if (enable) {
        enableProximity(true);

        // Gesture configuration
        write8(0xA0, 40);   // GPENTH: Enter threshold
        write8(0xA1, 30);   // GEXTH: Exit threshold
        write8(0xA2, 0x00); // GCONF1
        write8(0xA3, 0x41); // GCONF2: GGAIN 4x, 100mA, 2.8ms wait
        write8(0xA6, 0xC7); // GPULSE: 32us, 8 pulses
        write8(0xAB, 0x01); // GCONF4: GMODE = 1

        uint8_t en = read8(0x80);
        en |= 0x41; // PON + GEN
        write8(0x80, en);
    } else {
        uint8_t en = read8(0x80);
        en &= ~0x40;
        write8(0x80, en);
        write8(0xAB, 0x00);
    }
}

bool ArduLora_APDS9960::isGestureAvailable() {
    uint8_t gstatus = read8(0xAF);
    return (gstatus & 0x01) != 0; // GVALID bit
}

apds9960_gesture_t ArduLora_APDS9960::readGesture() {
    if (!isGestureAvailable()) {
        return DIR_NONE;
    }

    uint8_t fifo_level = read8(0xAE);
    if (fifo_level == 0) {
        return DIR_NONE;
    }

    int32_t sum_u = 0, sum_d = 0, sum_l = 0, sum_r = 0;
    int count = 0;

    for (int i = 0; i < fifo_level; i++) {
        _wire->beginTransmission(_addr);
        _wire->write(0xFC);
        _wire->endTransmission(false);
        if (_wire->requestFrom(_addr, (uint8_t)4) == 4) {
            uint8_t u = _wire->read();
            uint8_t d = _wire->read();
            uint8_t l = _wire->read();
            uint8_t r = _wire->read();
            sum_u += u;
            sum_d += d;
            sum_l += l;
            sum_r += r;
            count++;
        }
    }

    if (count == 0) return DIR_NONE;

    int32_t ud_diff = sum_u - sum_d;
    int32_t lr_diff = sum_l - sum_r;

    if (abs(ud_diff) > abs(lr_diff)) {
        if (ud_diff > 30) return DIR_UP;
        if (ud_diff < -30) return DIR_DOWN;
    } else {
        if (lr_diff > 30) return DIR_LEFT;
        if (lr_diff < -30) return DIR_RIGHT;
    }

    return DIR_NONE;
}
