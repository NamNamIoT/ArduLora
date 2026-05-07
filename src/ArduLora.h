/*
  ArduLora.h - Library for ArduLora (RAK3172) development.
  Created by NamNamIoT.
  Released into the public domain.
*/

#ifndef ArduLora_h
#define ArduLora_h

#include <Arduino.h>

// ArduLora Hardware Pins Mapping for RUI3
#define ARDULORA_SENSOR_POWER  PB5

// LEDs
#define ARDULORA_LED_YELLOW    PA8
#define ARDULORA_LED_RED       PA9
#define ARDULORA_LED_BLUE      PB2

// Functional Aliases for LEDs
#define ARDULORA_LED_STATUS    ARDULORA_LED_YELLOW
#define ARDULORA_LED_RX        ARDULORA_LED_BLUE
#define ARDULORA_LED_TX        ARDULORA_LED_RED

// Analog Inputs (0-10V range with onboard voltage divider)
#define ARDULORA_AI1           PB4
#define ARDULORA_AI2           PA10

// Battery Monitoring
#define ARDULORA_ADC_BAT       PB3

// I2C Interface
#define ARDULORA_SDA           PA11
#define ARDULORA_SCL           PA12

// RS485 / Modbus Interface (UART1)
#define ARDULORA_RS485_TX      PB6
#define ARDULORA_RS485_RX      PB7

// Backward Compatibility
#define Serial_Canopus         Serial1

class ArduLoraClass {
  public:
    ArduLoraClass();

    // 1. Hardware & Power Management
    void begin();
    void sensorPower(bool state);
    void setLed(uint8_t ledPin, bool state);

    // 2. Analog Inputs (returns millivolts)
    float readAI1_mV();
    float readAI2_mV();
    
    // 3. LoRa P2P Configuration Wrapper
    // Note: Calling this might reboot the board if it's currently in LoRaWAN mode.
    // Calling without arguments uses defaults: 868MHz, SF7, BW125, CR4/5, Preamble 8, TX Power 22.
    bool configLoraP2P(double freq = 868000000.0, uint16_t sf = 7, uint16_t bw = 0, uint16_t cr = 0, uint16_t preamble = 8, uint16_t txPower = 22);
    bool sendP2P(uint8_t *payload, uint16_t len);

    // 4. LoRaWAN Configuration Wrapper
    // Note: Calling this might reboot the board if it's currently in P2P mode.
    bool configLoRaWAN(uint8_t *devEui, uint8_t *appEui, uint8_t *appKey, uint8_t lorawanClass = 0, uint8_t region = 4);
    bool joinLoRaWAN(uint8_t join_mode = 1); // 1 = OTAA, 0 = ABP
    bool sendLoRaWAN(uint8_t port, uint8_t *payload, uint16_t len);

    // 5. System Functions
    void deepSleep(uint32_t time_ms);
    void reboot();
    String getChipID();
};

extern ArduLoraClass ArduLora;

#endif
