
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
double myFreq = 433000000;
//bw=0 is 125kHz, cr 0 is 4/5
uint16_t sf = 12, bw = 0, cr = 0, preamble = 8, txPower = 22;

void hexDump(uint8_t* buf, uint16_t len) {
  char alphabet[17] = "0123456789abcdef";
  Serial.print(F("   +------------------------------------------------+ +----------------+\r\n"));
  Serial.print(F("   |.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f | |      ASCII     |\r\n"));
  for (uint16_t i = 0; i < len; i += 16) {
    if (i % 128 == 0)
      Serial.print(F("   +------------------------------------------------+ +----------------+\r\n"));
    char s[] = "|                                                | |                |\r\n";
    uint8_t ix = 1, iy = 52;
    for (uint8_t j = 0; j < 16; j++) {
      if (i + j < len) {
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

void recv_cb(rui_lora_p2p_recv_t data) {
  rx_done = true;
  if (data.BufferSize == 0) {
    Serial.println("Empty buffer.");
    return;
  }
  char buff[92];
  sprintf(buff, "Incoming message, length: %d, RSSI: %d, SNR: %d",
          data.BufferSize, data.Rssi, data.Snr);
  Serial.println(buff);
  hexDump(data.Buffer, data.BufferSize);
}

void send_cb(void) {
  Serial.printf("P2P set Rx mode %s\r\n",
                api.lorawan.precv(3000) ? "Success" : "Fail");
}

void setup() {
  pinMode(PA8, OUTPUT);
  digitalWrite(PA8, HIGH);
  Serial.begin(115200);
  Serial.println("ArduLora LoRaWan P2P Example");
  Serial.println("------------------------------------------------------");
  delay(2000);
  startTime = millis();

  if (api.lorawan.nwm.get() != 0) {
    Serial.printf("Set Node device work mode %s\r\n",
                  api.lorawan.nwm.set(0) ? "Success" : "Fail");
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
                api.lorawan.pfreq.set(myFreq) ? "Success" : "Fail");
  Serial.printf("Set P2P mode spreading factor %d: %s\r\n", sf,
                api.lorawan.psf.set(sf) ? "Success" : "Fail");
  Serial.printf("Set P2P mode bandwidth %d: %s\r\n", bw,
                api.lorawan.pbw.set(bw) ? "Success" : "Fail");
  Serial.printf("Set P2P mode code rate 4/%d: %s\r\n", (cr + 5),
                api.lorawan.pcr.set(cr) ? "Success" : "Fail");
  Serial.printf("Set P2P mode preamble length %d: %s\r\n", preamble,
                api.lorawan.ppl.set(preamble) ? "Success" : "Fail");
  Serial.printf("Set P2P mode tx power %d: %s\r\n", txPower,
                api.lorawan.ptp.set(txPower) ? "Success" : "Fail");
  api.lorawan.registerPRecvCallback(recv_cb);
  api.lorawan.registerPSendCallback(send_cb);
  Serial.printf("P2P set Rx mode %s\r\n",
                api.lorawan.precv(3000) ? "Success" : "Fail");
  // let's kick-start things by waiting 3 seconds.
}

void loop() {
  uint8_t payload[] = "Hello Ra02";
  bool send_result = false;
  if (rx_done) {
    rx_done = false;
    while (!send_result) {
      send_result = api.lorawan.psend(sizeof(payload), payload);
      Serial.printf("P2P send %s\r\n", send_result ? "Success" : "Fail");
      if (!send_result) {
        Serial.printf("P2P finish Rx mode %s\r\n", api.lorawan.precv(0) ? "Success" : "Fail");
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
  
  
  
**Code on RA02_ESP32**  
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
#define BAND 433E6
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


  if (!LoRa.begin(433000000)) {
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
