#include <ArduLora.h>
/***
 *  This example print the device information.
***/

int i;

void setup()
{
    Serial.begin(115200);
    Serial.println("RAKwireless System General Example");
    Serial.println("------------------------------------------------------");
    api.system.restoreDefault();
}

void loop()
{
    if (++i == 20) {
        Serial.printf("Reboot now..\r\n");
        api.system.reboot();
    }
    Serial.printf("===Loop %d==\r\n", i);
    Serial.printf("Firmware Version: %s\r\n",
		api.system.firmwareVersion.get().c_str());
    Serial.printf("AT Command Version: %s\r\n",
		api.system.cliVersion.get().c_str());
    Serial.printf("RUI API Version: %s\r\n",
		api.system.apiVersion.get().c_str());
    Serial.printf("Model ID: %s\r\n", api.system.modelId.get().c_str());
    Serial.printf("Hardware ID: %s\r\n", api.system.chipId.get().c_str());
    Serial.printf("Battery Level: %f\r\n", api.system.bat.get());
    delay(1000);
}




