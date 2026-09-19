#ifndef __ARDULORA_APDS9960_H__
#define __ARDULORA_APDS9960_H__

#include <Arduino.h>
#include <Wire.h>

#define APDS9960_DEFAULT_ADDR 0x39

// Gesture enum
enum apds9960_gesture_t {
    DIR_NONE = 0,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN,
    DIR_NEAR,
    DIR_FAR
};

class ArduLora_APDS9960 {
  private:
    uint8_t _addr;
    TwoWire *_wire;

    uint8_t read8(uint8_t reg);
    uint16_t read16(uint8_t reg);
    void write8(uint8_t reg, uint8_t val);

  public:
    ArduLora_APDS9960(uint8_t addr = APDS9960_DEFAULT_ADDR, TwoWire *wire = &Wire);

    bool begin();
    
    // Proximity
    void enableProximity(bool enable = true);
    uint8_t readProximity(); // 0 (far) to 255 (near)

    // Color & Ambient light
    void enableColor(bool enable = true);
    bool readColor(uint16_t &r, uint16_t &g, uint16_t &b, uint16_t &c);

    // Gesture
    void enableGesture(bool enable = true);
    bool isGestureAvailable();
    apds9960_gesture_t readGesture();
};

#endif
