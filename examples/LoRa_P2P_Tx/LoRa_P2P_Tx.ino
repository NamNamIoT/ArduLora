#include <ArduLora.h>

long startTime;
bool rx_done = false;

void hexDump(uint8_t *buf, uint16_t len)
{
  for (uint16_t i = 0; i < len; i += 16)
  {
    char s[len];
    uint8_t iy = 0;
    for (uint8_t j = 0; j < 16; j++)
    {
      if (i + j < len)
      {
        uint8_t c = buf[i + j];
        if (c > 31 && c < 128)
          s[iy++] = c;
      }
    }

    String msg = String(s);
    Serial.println(msg);
  }
}

void recv_cb(rui_lora_p2p_recv_t data)
{
  rx_done = true;
  if (data.BufferSize == 0)
  {
    Serial.println("Empty buffer.");
    return;
  }
  ArduLora.setLed(ARDULORA_LED_RX, true); // Turn on RX LED (PB2)
  char buff[92];
  sprintf(buff, "Incoming message, length: %d, RSSI: %d, SNR: %d",
          data.BufferSize, data.Rssi, data.Snr);
  Serial.println(buff);
  hexDump(data.Buffer, data.BufferSize);
  ArduLora.setLed(ARDULORA_LED_RX, false); // Turn off RX LED
}

void send_cb(void)
{
  Serial.printf("P2P set Rx mode %s\r\n",
                api.lora.precv(65534) ? "Success" : "Fail");
}

void setup()
{
  Serial.begin(115200);
  Serial.println("ArduLora P2P TX Example (Using OOP Library)");
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

void loop()
{
  uint8_t payload[] = "payload";
  bool send_result = false;
  
  while (!send_result)
  {
    // Stop RX before sending
    api.lora.precv(0);
    
    // Send data
    send_result = ArduLora.sendP2P(payload, sizeof(payload));
    
    if (!send_result)
    {
      delay(1000);
    }
  }
  
  Serial.printf("P2P send Success\r\n");
  delay(1000);
  
  // Toggle TX LED
  static bool ledState = false;
  ledState = !ledState;
  ArduLora.setLed(ARDULORA_LED_TX, ledState);
}
