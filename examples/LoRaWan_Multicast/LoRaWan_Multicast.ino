#include <ArduLora.h>
/***
 *  This example shows LoRaWan protocol joining the network in OTAA mode, class C, region EU868 and set multicast group.
***/
bool ret;
void setup()
{
    Serial.begin(115200);
  
    Serial.println("RAKwireless LoRaWan Multicast Example");
    Serial.println("------------------------------------------------------");

    if(api.lorawan.nwm.get() != 1)
    {
        Serial.printf("Set Node device work mode %s\r\n",
            api.lorawan.nwm.set() ? "Success" : "Fail");
        api.system.reboot();
    }

    // OTAA Device EUI MSB
    uint8_t node_device_eui[8] =
        { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x88 };
    // OTAA Application EUI MSB
    uint8_t node_app_eui[8] =
        { 0x0E, 0x0D, 0x0D, 0x01, 0x0E, 0x01, 0x02, 0x0E };
    // OTAA Application Key MSB
    uint8_t node_app_key[16] =
        { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15,
        0x88, 0x09, 0xCF, 0x4F, 0x3E };
  
    /**LoRaWan Multicast Session*/
  
    uint8_t node_mc_address[4] = { 0x01, 0x02, 0x03, 0x04 };
    uint8_t node_mc_AppSKey[16] =
        { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07, 0x08 };
    uint8_t node_mc_NwkSKey[16] =
        { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07, 0x08 };
  
    RAK_LORA_McSession session = {
        .McDevclass = 2,
        .McAddress =
  	        node_mc_address[0] << 24 | node_mc_address[1] << 16 |
  	        node_mc_address[2] << 8 | node_mc_address[3],
        .McFrequency = 869525000,
        .McDatarate = 0,
        .McPeriodicity = 0,
        .McGroupID = 2,
        .entry = 0,
    };
    memcpy(session.McAppSKey, node_mc_AppSKey, 16);
    memcpy(session.McNwkSKey, node_mc_NwkSKey, 16);
  
    if (!(ret = api.lorawan.appeui.set(node_app_eui, 8))) {
        Serial.printf("LoRaWan Multicast - set device EUI is incorrect! \r\n");
        return;
    }
    if (!(ret = api.lorawan.appkey.set(node_app_key, 16))) {
        Serial.printf("LoRaWan Multicast - set application EUI is incorrect! \r\n");
        return;
    }
    if (!(ret = api.lorawan.deui.set(node_device_eui, 8))) {
        Serial.printf("LoRaWan Multicast - set application key is incorrect! \r\n");
        return;
    }
    /*************************************
     *
     * LoRaWAN band setting:
     *   EU433: 0
     *   CN470: 1
     *   RU864: 2
     *   IN865: 3
     *   EU868: 4
     *   US915: 5
     *   AU915: 6
     *   KR920: 7
     *   AS923: 8
     *
     * ************************************/
  
    if (!(ret = api.lorawan.band.set(4))) {
        Serial.printf("LoRaWan Multicast - set band is incorrect! \r\n");
        return;
    }
    if (!(ret = api.lorawan.njm.set(RAK_LORA_OTAA))) {
        Serial.printf("LoRaWan Multicast - set network join mode is incorrect! \r\n");
        return;
    }
    if (!(ret = api.lorawan.deviceClass.set(RAK_LORA_CLASS_C))) {
        Serial.printf("LoRaWan Multicast - set device class is incorrect! \r\n");
        return;
    }
  
    if (!(ret = api.lorawan.join())) {
        Serial.printf("LoRaWan Multicast - join fail! \r\n");
        return;
    }
  
    /**Wait for Join success */
    while (api.lorawan.njs.get() == 0) {
        Serial.print("Waiting for Lorawan join...");
        api.lorawan.join();
        delay(10000);
    }
  
    api.lorawan.adr.set(true);
    api.lorawan.rety.set(1);
    api.lorawan.cfm.set(1);
  
    /**LoRaWAN Multicast Setting*/
    if (api.lorawan.addmulc(session) == true) {
        Serial.println("Add Multicast Success");
    } else {
        Serial.println("Add Multicast Fail");
    }
}

void loop()
{
}




