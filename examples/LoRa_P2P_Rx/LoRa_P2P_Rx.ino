#include <ArduLora.h>

long startTime;
bool rx_done = false;

void recv_cb(rui_lora_p2p_recv_t data) {
  rx_done = true;
  if (data.BufferSize == 0) {
    Serial.println("Empty buffer.");
    return;
  }
  ArduLora.setLed(ARDULORA_LED_RX, true); // Turn on RX LED
  char buff[92];
  sprintf(buff, "Incoming message, length: %d, RSSI: %d, SNR: %d",
          data.BufferSize, data.Rssi, data.Snr);
  Serial.println(buff);
  ArduLora.setLed(ARDULORA_LED_RX, false); // Turn off RX LED
}

void send_cb(void) {
  Serial.printf("P2P set Rx mode %s\r\n",
                api.lora.precv(65534) ? "Success" : "Fail");
}

void setup() {
  Serial.begin(115200);
  Serial.println("ArduLora P2P RX Example (Using OOP Library)");
  Serial.println("------------------------------------------------------");
  
  // Initialize board and pins
  ArduLora.begin();
  ArduLora.sensorPower(true);
  
  startTime = millis();

  // Print Chip info
  Serial.printf("Hardware ID: %s\r\n", ArduLora.getChipID().c_str());

  // Configure LoRa P2P with default parameters: 868MHz, SF7, BW125, CR4/5, Preamble 8, TX Power 22
  if (ArduLora.configLoraP2P()) {
    Serial.println("LoRa P2P configured successfully!");
  } else {
    Serial.println("LoRa P2P configuration failed!");
  }

  // Register Callbacks
  api.lora.registerPRecvCallback(recv_cb);
  api.lora.registerPSendCallback(send_cb);
  
  // Start continuous receive mode
  api.lora.precv(65534);
}

void loop() {
  uint8_t payload[] = "ACK_from_ArduLora";
  bool send_result = false;
  
  if (rx_done) {
    rx_done = false;
    
    while (!send_result) {
      ArduLora.setLed(ARDULORA_LED_TX, true); // Turn on TX LED (PA9 equivalent)
      
      // Stop RX mode before sending
      api.lora.precv(0); 
      
      // Send ACK payload
      send_result = ArduLora.sendP2P(payload, sizeof(payload));
      Serial.printf("P2P send %s\r\n", send_result ? "Success" : "Fail");
      
      if (!send_result) {
        delay(1000);
      }
    }
    ArduLora.setLed(ARDULORA_LED_TX, false); // Turn off TX LED
  }
  
  delay(500);
  
  // Toggle Status LED
  static bool ledState = false;
  ledState = !ledState;
  ArduLora.setLed(ARDULORA_LED_STATUS, ledState);
}
