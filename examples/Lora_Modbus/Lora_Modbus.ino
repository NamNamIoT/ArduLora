#include "function.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("RAK3172_Canopus LoRa P2P Example");
  Serial.println("------------------------------------------------------");
  pinMode(PB5, OUTPUT);
  digitalWrite(PB5, HIGH);
  init_lora(868000000);
  Serial_modbus.begin(9600, RAK_CUSTOM_MODE);
  Serial_modbus.println("RAK3172_Canopus LoRa_Modbus tunnel Example");
}

void loop()
{
  uint8_t frame_input[100];
  int size_input = 0;
  while (Serial_modbus.available())
  {
    frame_input[size_input++] = Serial_modbus.read();
  }
  if (size_input != 0)
  {
    lora_send(size_input, frame_input);
  }
  delay(100);
}
