
# ArduLora communication with Lora Ra02
  
<img src="https://github.com/NamNamIoT/ArduLora/assets/49629370/7da1e890-1e2c-4b1b-82d6-35317b26b6f7" height="450"><img src="https://user-images.githubusercontent.com/49629370/64397342-2c133a80-d08b-11e9-9520-96f3bfae00c6.jpg" height="450">

🧑🏻‍🔧Setting:  
SetPreambleLength(8);  
SetSpreadingFactor(12);  
SetSignalBandwidth(125E3);  //125KHz  
SetCodingRate4(5);  
  
**Code on ArduLora board**

```c
long startTime;
bool rx_done = false;
double myFreq = 868000000;
// bw=0 is 125kHz, cr 0 is 4/5
uint16_t sf = 12, bw = 0, cr = 0, preamble = 8, txPower = 22;

void hexDump(uint8_t *buf, uint16_t len)
{
  char alphabet[17] = "0123456789abcdef";
  Serial.print(F("   +------------------------------------------------+ +----------------+\r\n"));
  Serial.print(F("   |.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f | |      ASCII     |\r\n"));
  for (uint16_t i = 0; i < len; i += 16)
  {
    if (i % 128 == 0)
      Serial.print(F("   +------------------------------------------------+ +----------------+\r\n"));
    char s[] = "|                                                | |                |\r\n";
    uint8_t ix = 1, iy = 52;
    for (uint8_t j = 0; j < 16; j++)
    {
      if (i + j < len)
      {
        uint8_t c = buf[i + j];
        s[ix++] = alphabet[(c >> 4) & 0x0F];
        s[ix++] = alphabet[c & 0x0F];
        ix++;
        if (c > 31 && c < 128)
          s[iy++] = c;
        else
          s[iy++] = '.';
      }
    }
    uint8_t index = i / 16;
    if (i < 256)
      Serial.write(' ');
    Serial.print(index, HEX);
    Serial.write('.');
    Serial.print(s);
  }
  Serial.print(F("   +------------------------------------------------+ +----------------+\r\n"));
}

void recv_cb(rui_lora_p2p_recv_t data)
{
  rx_done = true;
  if (data.BufferSize == 0)
  {
    Serial.println("Empty buffer.");
    return;
  }
  char buff[92];
  sprintf(buff, "Incoming message, length: %d, RSSI: %d, SNR: %d",
          data.BufferSize, data.Rssi, data.Snr);
  Serial.println(buff);
  hexDump(data.Buffer, data.BufferSize);
}

void send_cb(void)
{
  Serial.printf("P2P set Rx mode %s\r\n",
                api.lora.precv(3000) ? "Success" : "Fail");
}

void setup()
{
  pinMode(PB5, OUTPUT);
  digitalWrite(PB5, LOW);
  Serial.begin(115200);
  Serial.println("RAK3172_Canopus lora P2P Example");
  Serial.println("------------------------------------------------------");
  delay(2000);
  startTime = millis();

  if (api.lora.nwm.get() != 0)
  {
    Serial.printf("Set Node device work mode %s\r\n",
                  api.lora.nwm.set() ? "Success" : "Fail");
    api.system.reboot();
  }

  Serial.println("P2P Start");
  Serial.printf("Hardware ID: %s\r\n", api.system.chipId.get().c_str());
  Serial.printf("Model ID: %s\r\n", api.system.modelId.get().c_str());
  Serial.printf("RUI API Version: %s\r\n",
                api.system.apiVersion.get().c_str());
  Serial.printf("Firmware Version: %s\r\n",
                api.system.firmwareVersion.get().c_str());
  Serial.printf("AT Command Version: %s\r\n",
                api.system.cliVersion.get().c_str());
  Serial.printf("Set P2P mode frequency %3.3f: %s\r\n", (myFreq / 1e6),
                api.lora.pfreq.set(myFreq) ? "Success" : "Fail");
  Serial.printf("Set P2P mode spreading factor %d: %s\r\n", sf,
                api.lora.psf.set(sf) ? "Success" : "Fail");
  Serial.printf("Set P2P mode bandwidth %d: %s\r\n", bw,
                api.lora.pbw.set(bw) ? "Success" : "Fail");
  Serial.printf("Set P2P mode code rate 4/%d: %s\r\n", (cr + 5),
                api.lora.pcr.set(cr) ? "Success" : "Fail");
  Serial.printf("Set P2P mode preamble length %d: %s\r\n", preamble,
                api.lora.ppl.set(preamble) ? "Success" : "Fail");
  Serial.printf("Set P2P mode tx power %d: %s\r\n", txPower,
                api.lora.ptp.set(txPower) ? "Success" : "Fail");
  api.lora.registerPRecvCallback(recv_cb);
  api.lora.registerPSendCallback(send_cb);
  Serial.printf("P2P set Rx mode %s\r\n",
                api.lora.precv(3000) ? "Success" : "Fail");
  // let's kick-start things by waiting 3 seconds.
}

void loop()
{
  uint8_t payload[] = "Hello Ra02";
  bool send_result = false;
  if (rx_done)
  {
    rx_done = false;
    while (!send_result)
    {
      send_result = api.lora.psend(sizeof(payload), payload);
      Serial.printf("P2P send %s\r\n", send_result ? "Success" : "Fail");
      if (!send_result)
      {
        Serial.printf("P2P finish Rx mode %s\r\n", api.lora.precv(0) ? "Success" : "Fail");
        delay(1000);
      }
    }
  }
  delay(2000);
}  

```
  
The Arduino Serial Monitor shows :

```c
02:40:53.283 -> Incoming message, length: 13, RSSI: -28, SNR: 6
02:40:53.283 ->    +------------------------------------------------+ +----------------+
02:40:53.327 ->    |.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f | |      ASCII     |
02:40:53.327 ->    +------------------------------------------------+ +----------------+
02:40:53.327 ->  0.|48 65 6c 6c 6f 20 52 61 6b 33 31 37 32          | |Hello ArduLora   |
02:40:53.327 ->    +------------------------------------------------+ +----------------+
02:40:53.624 -> P2P send Success
02:40:54.764 -> P2P set Rx mode Success
02:40:54.764 -> +EVT:TXP2P DONE
02:40:57.462 -> +EVT:RXP2P:-28:5:48656C6C6F2052616B33313732
```
  
  
  
**Code on RA-02 vs_ESP32**  
```c
#include <LoRa.h>
#include <SPI.h>
int counter = 0;
//Ra02 - ESP32
#define SCK 18   //  SX1278's SCK
#define MISO 19  //  SX1278's MISO
#define MOSI 23  //  SX1278's MOSI
#define SS 5     //  SX1278's CS
#define RST 14   //  SX1278's RESET
#define DI0 2    //  SX1278's IRQ(Interrupt Request)
#define BAND 868E6
String outgoing;  // outgoing message

byte localAddress = 0xBB;  // address of this device
byte destination = 0xFF;   // destination to send to
long lastSendTime = 0;     // last send time
int interval = 2000;       // interval between sends
void setup() {
  delay(10000);

  Serial.begin(9600);
  while (!Serial) {
    delay(1);
  }

  randomSeed(analogRead(0));
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);


  if (!LoRa.begin(868000000)) {
    Serial.println("Starting LoRa-Receiver failed!");
    while (1) {
      delay(1);
    };
  }

  LoRa.setPreambleLength(8);
  LoRa.setSpreadingFactor(12);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setTxPower(17);

  Serial.println("LoRa init succeeded.");
}

void loop() {
  if (millis() - lastSendTime > interval) {
    String message = "Hello ArduLora";  // send a message
    sendMessage(message);
    Serial.println("Sending " + message);
    lastSendTime = millis();  // timestamp the message
    interval = 3000;          // 2-3 seconds
  }

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();    // start packet
  LoRa.print(outgoing);  // add payload
  LoRa.endPacket();      // finish packet and send it
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;  // if there's no packet, return

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }
  incoming = incoming.substring(0, incoming.length() - 1);
  Serial.println("Recieve: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
}
```
  
The Arduino Serial Monitor shows :

```c
02:38:56.907 -> Sending Hello ArduLora
02:38:58.180 -> Recieve: Hello Ra02
02:38:58.212 -> RSSI: -31
02:38:58.212 -> Snr: 9.50
02:38:58.249 -> 
02:39:01.063 -> Sending Hello ArduLora
02:39:02.705 -> Recieve: Hello Ra02
02:39:02.705 -> RSSI: -31
02:39:02.751 -> Snr: 10.25
```

**Code on RA-01SH vs ESP32**  
```c
#include <RadioLib.h>
SX1262 radio = new Module(5, 2, 14, -1);
void setup() {
  Serial.begin(115200);

  // initialize SX1268 with default settings
  Serial.print(F("[SX1262] Initializing ... "));
  int state = radio.begin(868, 125, 9, 7, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 10, 8, 0, true);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }


  // you can also change the settings at runtime
  // and check if the configuration was changed successfully

  // set carrier frequency to 868 MHz
  if (radio.setFrequency(868) == RADIOLIB_ERR_INVALID_FREQUENCY) {
    Serial.println(F("Selected frequency is invalid for this module!"));
    while (true) { delay(10); }
  }

  // set bandwidth to 125 kHz
  if (radio.setBandwidth(125.0) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
    Serial.println(F("Selected bandwidth is invalid for this module!"));
    while (true) { delay(10); }
  }

  // set spreading factor to 12
  if (radio.setSpreadingFactor(12) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
    Serial.println(F("Selected spreading factor is invalid for this module!"));
    while (true) { delay(10); }
  }

  // set coding rate to 5
  if (radio.setCodingRate(5) == RADIOLIB_ERR_INVALID_CODING_RATE) {
    Serial.println(F("Selected coding rate is invalid for this module!"));
    while (true) { delay(10); }
  }

  // // set LoRa sync word to 0xAB
  // if (radio1.setSyncWord(0xAB) != RADIOLIB_ERR_NONE) {
  //   Serial.println(F("Unable to set sync word!"));
  //   while (true) { delay(10); }
  // }

  // set output power to 10 dBm (accepted range is -17 - 22 dBm)
  if (radio.setOutputPower(22) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
    Serial.println(F("Selected output power is invalid for this module!"));
    while (true) { delay(10); }
  }

  // set over current protection limit to 80 mA (accepted range is 45 - 240 mA)
  // NOTE: set value to 0 to disable overcurrent protection
  if (radio.setCurrentLimit(80) == RADIOLIB_ERR_INVALID_CURRENT_LIMIT) {
    Serial.println(F("Selected current limit is invalid for this module!"));
    while (true) { delay(10); }
  }

  // set LoRa preamble length to 15 symbols (accepted range is 0 - 65535)
  if (radio.setPreambleLength(8) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
    Serial.println(F("Selected preamble length is invalid for this module!"));
    while (true) { delay(10); }
  }

  // // disable CRC
  // if (radio1.setCRC(false) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION) {
  //   Serial.println(F("Selected CRC is invalid for this module!"));
  //   while (true) { delay(10); }
  // }

  // Some SX126x modules have TCXO (temperature compensated crystal
  // oscillator). To configure TCXO reference voltage,
  // the following method can be used.
  // if (radio1.setTCXO(2.4) == RADIOLIB_ERR_INVALID_TCXO_VOLTAGE) {
  //   Serial.println(F("Selected TCXO voltage is invalid for this module!"));
  //   while (true) { delay(10); }
  // }

  // Some SX126x modules use DIO2 as RF switch. To enable
  // this feature, the following method can be used.
  // NOTE: As long as DIO2 is configured to control RF switch,
  //       it can't be used as interrupt pin!
  // if (radio1.setDio2AsRfSwitch() != RADIOLIB_ERR_NONE) {
  //   Serial.println(F("Failed to set DIO2 as RF switch!"));
  //   while (true) { delay(10); }
  // }

  Serial.println(F("All settings succesfully changed!"));
}

void loop() {
  String str;
  int state = radio.receive(str);
  if (state == RADIOLIB_ERR_NONE) {
    // packet was successfully received
    Serial.println(F("success!"));

    // print the data of the packet
    Serial.print(F("[SX1262] Data:\t\t"));
    Serial.println(str);

    // print the RSSI (Received Signal Strength Indicator)
    // of the last received packet
    Serial.print(F("[SX1262] RSSI:\t\t"));
    Serial.print(radio.getRSSI());
    Serial.println(F(" dBm"));

    // print the SNR (Signal-to-Noise Ratio)
    // of the last received packet
    Serial.print(F("[SX1262] SNR:\t\t"));
    Serial.print(radio.getSNR());
    Serial.println(F(" dB"));

    // print frequency error
    Serial.print(F("[SX1262] Frequency error:\t"));
    Serial.print(radio.getFrequencyError());
    Serial.println(F(" Hz"));
  }
}
```
  
The Arduino Serial Monitor shows :

```c
00:08:09.262 -> success!
00:08:09.262 -> [SX1262] Data:		Hello Ra02
00:08:09.262 -> [SX1262] RSSI:		-34.00 dBm
00:08:09.307 -> [SX1262] SNR:		5.50 dB
00:08:09.399 -> [SX1262] Frequency error:	-16989.21 Hz
00:08:21.245 -> success!
00:08:21.245 -> [SX1262] Data:		Hello Ra02
00:08:21.245 -> [SX1262] RSSI:		-32.00 dBm
00:08:21.290 -> [SX1262] SNR:		5.25 dB
00:08:21.377 -> [SX1262] Frequency error:	-16994.54 Hz
00:08:33.249 -> success!
00:08:33.249 -> [SX1262] Data:		Hello Ra02
00:08:33.249 -> [SX1262] RSSI:		-32.00 dBm
00:08:33.295 -> [SX1262] SNR:		5.50 dB
00:08:33.393 -> [SX1262] Frequency error:	-17003.74 Hz
00:08:45.231 -> success!
00:08:45.231 -> [SX1262] Data:		Hello Ra02
00:08:45.273 -> [SX1262] RSSI:		-36.00 dBm
00:08:45.319 -> [SX1262] SNR:		5.75 dB
00:08:45.411 -> [SX1262] Frequency error:	-17022.63 Hz
```
