#ifndef __ARTRONSHOP_SHT3X_H__
#define __ARTRONSHOP_SHT3X_H__

#include <Arduino.h>
#include <Wire.h>

#define SHT3X_ADDR0 (0x44)
#define SHT3X_ADDR1 (0x45)

class ArtronShop_SHT3x {
    private:
        uint8_t _addr;
        TwoWire *_wire = NULL;
        float _t, _h;

    public:
        ArtronShop_SHT3x(uint8_t addr = SHT3X_ADDR0, TwoWire *wire = &Wire) ;
        bool begin() ;
        bool measure() ;
        float temperature() ;
        float humidity() ;

};

#endif
