#include "ArduLora.h"

ArduLoraClass::ArduLoraClass() {
    // Constructor
}

void ArduLoraClass::begin() {
    // Initialize power control pin
    pinMode(ARDULORA_SENSOR_POWER, OUTPUT);
    digitalWrite(ARDULORA_SENSOR_POWER, HIGH);

    // Initialize LEDs
    pinMode(ARDULORA_LED_YELLOW, OUTPUT);
    pinMode(ARDULORA_LED_RED, OUTPUT);
    pinMode(ARDULORA_LED_BLUE, OUTPUT);

    // Increase ADC resolution for better analog readings
    analogReadResolution(12);
}

void ArduLoraClass::sensorPower(bool state) {
    digitalWrite(ARDULORA_SENSOR_POWER, state ? LOW : HIGH);
}

void ArduLoraClass::setLed(uint8_t ledPin, bool state) {
    digitalWrite(ledPin, state ? HIGH : LOW);
}

float ArduLoraClass::readAI1_mV() {
    // Reading raw value and multiplying by the voltage divider coefficient (approx 2.58)
    return analogRead(ARDULORA_AI1) * 2.58; 
}

float ArduLoraClass::readAI2_mV() {
    return analogRead(ARDULORA_AI2) * 2.58;
}

bool ArduLoraClass::configLoraP2P(double freq, uint16_t sf, uint16_t bw, uint16_t cr, uint16_t preamble, uint16_t txPower) {
    // Switch to P2P mode if currently in LoRaWAN
    if (api.lora.nwm.get() != 0) {
        api.lora.nwm.set(0); 
        api.system.reboot(); // Reboot is required after changing network work mode
    }

    bool success = true;
    success &= api.lora.pfreq.set(freq);
    success &= api.lora.psf.set(sf);
    success &= api.lora.pbw.set(bw);
    success &= api.lora.pcr.set(cr);
    success &= api.lora.ppl.set(preamble);
    success &= api.lora.ptp.set(txPower);

    return success;
}

bool ArduLoraClass::sendP2P(uint8_t *payload, uint16_t len) {
    return api.lora.psend(len, payload);
}

bool ArduLoraClass::configLoRaWAN(uint8_t *devEui, uint8_t *appEui, uint8_t *appKey, uint8_t lorawanClass, uint8_t region) {
    // Switch to LoRaWAN mode if currently in P2P mode
    if (api.lora.nwm.get() != 1) {
        api.lora.nwm.set(1); 
        api.system.reboot(); // Reboot is required after changing network work mode
    }

    bool success = true;
    success &= api.lora.deveui.set(devEui, 8);
    success &= api.lora.appeui.set(appEui, 8);
    success &= api.lora.appkey.set(appKey, 16);
    success &= api.lora.deviceClass.set(lorawanClass);
    success &= api.lora.band.set(region);
    
    return success;
}

bool ArduLoraClass::joinLoRaWAN(uint8_t join_mode) {
    api.lora.njm.set(join_mode); // 1 for OTAA, 0 for ABP
    return api.lora.join();
}

bool ArduLoraClass::sendLoRaWAN(uint8_t port, uint8_t *payload, uint16_t len) {
    return api.lora.send(len, payload, port);
}

void ArduLoraClass::deepSleep(uint32_t time_ms) {
    api.system.sleep.all(time_ms);
}

void ArduLoraClass::reboot() {
    api.system.reboot();
}

String ArduLoraClass::getChipID() {
    return api.system.chipId.get();
}

// Instantiate the global object
ArduLoraClass ArduLora;
