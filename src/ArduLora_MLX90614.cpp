#include "ArduLora_MLX90614.h"

ArduLora_MLX90614::ArduLora_MLX90614(uint8_t addr, TwoWire *wire) : _addr(addr), _wire(wire) {
}

bool ArduLora_MLX90614::begin() {
    // Check if device responds
    _wire->beginTransmission(_addr);
    return (_wire->endTransmission() == 0);
}

float ArduLora_MLX90614::readTemp(uint8_t reg) {
    _wire->beginTransmission(_addr);
    _wire->write(reg);
    if (_wire->endTransmission(false) != 0) {
        return -999.0f;
    }

    if (_wire->requestFrom(_addr, (uint8_t)3) != 3) {
        return -999.0f;
    }

    uint8_t lsb = _wire->read();
    uint8_t msb = _wire->read();
    uint8_t pec = _wire->read();
    (void)pec; // Optional CRC check

    uint16_t raw = ((uint16_t)msb << 8) | lsb;

    // Error flag bit 15 on MLX90614
    if (raw & 0x8000) {
        return -999.0f;
    }

    float kelvin = (float)raw * 0.02f;
    return kelvin - 273.15f;
}

float ArduLora_MLX90614::readAmbientTempC() {
    return readTemp(MLX90614_RAW_TA);
}

float ArduLora_MLX90614::readObjectTempC() {
    return readTemp(MLX90614_RAW_TOBJ1);
}
