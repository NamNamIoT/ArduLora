// RAK3172 LoRaWAN ABP Example

#include <LoRaWAN.h>

// LoRaWAN credentials
const char *devAddr = "260B7EF0";  // Device Address
const char *nwkSKey = "7A7BFA56709DC7DE1D2BCE9D85CFEA98";  // Network Session Key
const char *appSKey = "33B3040FA48D7D550041A0F295623708";  // Application Session Key

LoRaWAN lora;  // Create LoRaWAN instance

void setup() {
    Serial.begin(9600);
    // Initialize LoRaWAN 
    lora.begin();
    lora.setDevAddr((uint8_t*)devAddr);
    lora.setNwkSKey((uint8_t*)nwkSKey);
    lora.setAppSKey((uint8_t*)appSKey);
    Serial.println("RAK3172 ABP example setup complete");
}

void loop() {
    // Example of sending a message
    String message = "Hello, LoRaWAN!";
    if (lora.send(message.c_str())) {
        Serial.println("Message sent: " + message);
    } else {
        Serial.println("Failed to send message");
    }
    delay(10000);  // Wait for 10 seconds before sending again
}