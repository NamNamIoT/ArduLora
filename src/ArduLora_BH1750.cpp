#include "ArduLora_BH1750.h"

ArduLora_BH1750::ArduLora_BH1750(int addr, TwoWire *wire) : _wire(wire), _addr(addr) {
    // ----
}

bool ArduLora_BH1750::begin() {
    _wire->beginTransmission(this->_addr);
    _wire->write(0x10); // Continuously H-Resolution Mode
    if (_wire->endTransmission() == 0) {
        delay(180); // Wait to complete 1st H-resolution mode measurement. (max. 180ms.) 
        return true;
    }

    return false;
}

float ArduLora_BH1750::light() {
    int n = _wire->requestFrom(this->_addr, 2);
    if (n != 2) {
        return -1.0f;
    }

    uint8_t data[2];
    _wire->readBytes(data, 2);

    return ((uint16_t)(((uint16_t) data[0] << 8) | data[1])) / 1.2f;
}
