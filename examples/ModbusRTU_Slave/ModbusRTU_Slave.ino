#include <Rak3172_Canopus.h>
#include "modbus.h"
#include "modbusDevice.h"
#include "modbusRegBank.h"
#include "modbusSlave.h"
modbusDevice regBank;
modbusSlave slave;

void setup()
{
  init_io();
  enable_Vrs485();
  Serial.begin(115200);
  Serial.print("\r\n*****************RAK3172_CANOPUS*******************");

  regBank.setId(1);      // Set id slave
  regBank.add(40001);    // Add register FC03, holding register, address 1
  regBank.set(40001, 0); // Set default value for 40001 is 0
  slave._device = &regBank;
  slave.setBaud(9600); // Set baudrate 9600

  analogReadResolution(12); // Set Resolution adc is 12bit, can upto 14bit
}
void loop()
{
  int analog_In = analogRead(AI1_PIN);

  regBank.set(40001, analog_In);                      // Update value for 40001 is analog_In
  slave.run();                                        // Run service modbus RTU slave
  digitalWrite(LED_SYNC, !digitalRead(LED_SYNC)); // blink led
  delay(200);
}
