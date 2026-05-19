#include "ArtronShop_SHT3x.h"

ArtronShop_SHT3x::ArtronShop_SHT3x(uint8_t addr, TwoWire *wire) : _addr(addr), _wire(wire) {
    // -----
}

bool ArtronShop_SHT3x::begin() {
    // periodic data acquisition mode
    this->_wire->beginTransmission(this->_addr);
    this->_wire->write(0x27); // mps => 10
    this->_wire->write(0x37); // Repeatability => High
    return this->_wire->endTransmission() == 0;
}

bool ArtronShop_SHT3x::measure() {
    this->_wire->beginTransmission(this->_addr);
    this->_wire->write(0xE0);
    this->_wire->write(0x00);
    if (this->_wire->endTransmission(false) != 0) {
        this->_t = -99;
        this->_h = -99;
        return false;
    }

    if (this->_wire->requestFrom(this->_addr, (uint8_t)6) != 6) {
        this->_t = -99;
        this->_h = -99;
        return false;
    }

    uint8_t rx_bytes[6];
    memset(rx_bytes, 0, 6);
    this->_wire->readBytes(rx_bytes, 6);

    uint16_t t_ticks = ((uint16_t)rx_bytes[0] << 8) | rx_bytes[1];
    // checksum_t = rx_bytes[2];
    uint16_t rh_ticks = ((uint16_t)rx_bytes[3] << 8) | rx_bytes[4];
    // checksum_rh = rx_bytes[5];
    this->_t = -45.0f + (175.0f * (t_ticks / 65535.0f));
    this->_h = 100 * (rh_ticks / 65535.0f);
    if (this->_h > 100) {
        this->_h = 100;
    }
    if (this->_h < 0) {
        this->_h = 0;
    }

    return true;
}

float ArtronShop_SHT3x::temperature() {
    return this->_t;
}

float ArtronShop_SHT3x::humidity() {
    return this->_h;
}
