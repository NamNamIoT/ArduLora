#ifndef __ARTRONSHOP_BH1750__
#define __ARTRONSHOP_BH1750__

#include <Arduino.h>
#include <Wire.h>

class ArtronShop_BH1750 {
    private:
        TwoWire *_wire;
        int _addr;

    public:
        ArtronShop_BH1750(int addr = 0x5C, TwoWire *wire = &Wire) ;

        bool begin() ;
        float light() ;

};

#endif
