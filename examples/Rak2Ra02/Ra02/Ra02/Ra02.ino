#include <ArduLora.h>
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
    String message = "Hello Rak3172";  // send a message
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



