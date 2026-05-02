#include <ArduLora.h>
/***
 *  This example shows LoRaWan protocol joining the network in ABP mode, class A, region US915.
 *  Device will send uplink every 5 seconds.
***/

#define ABP_PERIOD   (5000)
/*************************************

   LoRaWAN band setting:
     RAK_REGION_EU433
     RAK_REGION_CN470
     RAK_REGION_RU864
     RAK_REGION_IN865
     RAK_REGION_EU868
     RAK_REGION_US915
     RAK_REGION_AU915
     RAK_REGION_KR920
     RAK_REGION_AS923

 *************************************/
#define ABP_BAND     (RAK_REGION_US915)
#define ABP_DEVADDR  {0x05, 0x05, 0x06, 0x06}
#define ABP_APPSKEY  {0xB4, 0x91, 0xCC, 0x10, 0x17, 0x0E, 0x89, 0x04, 0x33, 0xCA, 0x5B, 0x13, 0x1E, 0x74, 0x20, 0x07}
#define ABP_NWKSKEY  {0xBF, 0x9B, 0x75, 0xBC, 0xD6, 0x08, 0x06, 0xDD, 0x80, 0xED, 0xB8, 0xE6, 0x83, 0x29, 0x9D, 0x22}

/** Packet buffer for sending */
uint8_t collected_data[64] = { 0 };

void recvCallback(SERVICE_LORA_RECEIVE_T * data)
{
    if (data->BufferSize > 0) {
        Serial.println("Something received!");
        for (int i = 0; i < data->BufferSize; i++) {
            Serial.printf("%x", data->Buffer[i]);
        }
        Serial.print("\r\n");
    }
}

void sendCallback(int32_t status)
{
    if (status == 0) {
        Serial.println("Successfully sent");
    } else {
        Serial.println("Sending failed");
    }
}

void setup()
{
    Serial.begin(115200, RAK_AT_MODE);
  
    Serial.println("RAKwireless LoRaWan ABP Example");
    Serial.println("------------------------------------------------------");

    if(api.lorawan.nwm.get() != 1)
    {
        Serial.printf("Set Node device work mode %s\r\n",
            api.lorawan.nwm.set() ? "Success" : "Fail");
        api.system.reboot();
    }

    // ABP Device Address MSB first
    uint8_t node_dev_addr[4] = ABP_DEVADDR;
    // ABP Application Session Key
    uint8_t node_app_skey[16] = ABP_APPSKEY;
    // ABP Network Session Key
    uint8_t node_nwk_skey[16] = ABP_NWKSKEY;
  
    if (!api.lorawan.daddr.set(node_dev_addr, 4)) {
        Serial.printf("LoRaWan ABP - set device addr is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.appskey.set(node_app_skey, 16)) {
        Serial.printf("LoRaWan ABP - set application session key is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.nwkskey.set(node_nwk_skey, 16)) {
        Serial.printf("LoRaWan ABP - set network session key is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.band.set(ABP_BAND)) {
        Serial.printf("LoRaWan ABP - set band is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.deviceClass.set(RAK_LORA_CLASS_A)) {
        Serial.printf("LoRaWan ABP - set device class is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.njm.set(RAK_LORA_ABP))	// Set the network join mode to ABP
    {
        Serial.printf("LoRaWan ABP - set network join mode is incorrect! \r\n");
        return;
    }
  
    if (!api.lorawan.adr.set(true)) {
        Serial.printf("LoRaWan ABP - set adaptive data rate is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.rety.set(1)) {
        Serial.printf("LoRaWan ABP - set retry times is incorrect! \r\n");
        return;
    }
    if (!api.lorawan.cfm.set(1)) {
        Serial.printf("LoRaWan ABP - set confirm mode is incorrect! \r\n");
        return;
    }
  
    /** Check LoRaWan Status*/
    Serial.printf("Duty cycle is %s\r\n", api.lorawan.dcs.get()? "ON" : "OFF");	// Check Duty Cycle status
    Serial.printf("Packet is %s\r\n", api.lorawan.cfm.get()? "CONFIRMED" : "UNCONFIRMED");	// Check Confirm status
    uint8_t assigned_dev_addr[4] = { 0 };
    api.lorawan.daddr.get(assigned_dev_addr, 4);
    Serial.printf("Device Address is %02X%02X%02X%02X\r\n", assigned_dev_addr[0], assigned_dev_addr[1], assigned_dev_addr[2], assigned_dev_addr[3]);	// Check Device Address
    Serial.printf("Uplink period is %ums\r\n", ABP_PERIOD);
    Serial.println("");
    api.lorawan.registerRecvCallback(recvCallback);
    api.lorawan.registerSendCallback(sendCallback);
}

void uplink_routine()
{
    /** Payload of Uplink */
    uint8_t data_len = 0;
    collected_data[data_len++] = (uint8_t) 't';
    collected_data[data_len++] = (uint8_t) 'e';
    collected_data[data_len++] = (uint8_t) 's';
    collected_data[data_len++] = (uint8_t) 't';
  
    Serial.println("Data Packet:");
    for (int i = 0; i < data_len; i++) {
        Serial.printf("0x%02X ", collected_data[i]);
    }
    Serial.println("");
  
    /** Send the data package */
    if (api.lorawan.send(data_len, (uint8_t *) & collected_data, 2, true, 1)) {
        Serial.println("Sending is requested");
    } else {
        Serial.println("Sending failed");
    }
}

void loop()
{
    static uint64_t last = 0;
    static uint64_t elapsed;
  
    if ((elapsed = millis() - last) > ABP_PERIOD) {
        uplink_routine();
  
        last = millis();
    }
    //Serial.printf("Try sleep %ums..", ABP_PERIOD);
    api.system.sleep.all(ABP_PERIOD);
    //Serial.println("Wakeup..");
}




