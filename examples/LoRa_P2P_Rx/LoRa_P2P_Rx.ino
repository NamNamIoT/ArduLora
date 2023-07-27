#define LED_SEND PA8
#define LED_RECV PB2
#define LED_SYNC PA0
long startTime;
bool rx_done = false;
double myFreq = 433000000;
uint16_t sf = 12, bw = 0, cr = 0, preamble = 8, txPower = 22;

void recv_cb(rui_lora_p2p_recv_t data) {
  rx_done = true;
  if (data.BufferSize == 0) {
    Serial.println("Empty buffer.");
    return;
  }
  digitalWrite(LED_RECV, HIGH);
  char buff[92];
  sprintf(buff, "Incoming message, length: %d, RSSI: %d, SNR: %d",
          data.BufferSize, data.Rssi, data.Snr);
  Serial.println(buff);
  digitalWrite(LED_RECV, LOW);
}

void send_cb(void) {
  Serial.printf("P2P set Rx mode %s\r\n",
                api.lorawan.precv(65534) ? "Success" : "Fail");
}

void setup() {
  pinMode(LED_SEND, OUTPUT);
  pinMode(LED_RECV, OUTPUT);
  pinMode(LED_SYNC, OUTPUT);
  Serial.begin(115200);
  Serial.println("RAK3172_Canopus LoRaWan P2P Example");
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
                api.lorawan.precv(65534) ? "Success" : "Fail");
}

void loop() {
  uint8_t payload[] = "payload_B";
  bool send_result = false;
  if (rx_done) {
    rx_done = false;
    while (!send_result) {
      digitalWrite(LED_SEND, HIGH);
      send_result = api.lorawan.psend(sizeof(payload), payload);
      Serial.printf("P2P send %s\r\n", send_result ? "Success" : "Fail");
      if (!send_result) {
        Serial.printf("P2P finish Rx mode %s\r\n", api.lorawan.precv(0) ? "Success" : "Fail");
        delay(1000);
      }
    }
    digitalWrite(LED_SEND, LOW);
  }
  delay(500);
  digitalWrite(LED_SYNC, !digitalRead(LED_SYNC));
}
