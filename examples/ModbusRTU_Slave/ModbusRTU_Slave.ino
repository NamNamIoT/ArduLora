#include "modbus.h"
#include "modbusDevice.h"
#include "modbusRegBank.h"
#include "modbusSlave.h"
modbusDevice regBank;
modbusSlave slave;

void setup()
{
  //Enable power for external sensor
  pinMode(PB5, OUTPUT);
  digitalWrite(PB5, HIGH);

  //Led PA8 as output
  pinMode(PA8, OUTPUT);
  Serial.begin(115200);
  Serial.print("\r\n*****************ArduLora*******************");
  
  regBank.setId(1);  //Set id slave
  regBank.add(40001);  //Add register FC03, holding register, address 1
  regBank.set(40001,0);  //Set default value for 40001 is 0
  slave._device = &regBank;
  slave.setBaud(9600); //Set baudrate 9600
  
  analogReadResolution(12);  //Set Resolution adc is 12bit, can upto 14bit
}
void loop()
{
  int analog_In = analogRead(PB4); 
  
  regBank.set(40001, analog_In);  //Update value for 40001 is analog_In
  slave.run();  //Run service modbus RTU slave
  digitalWrite(PA8, !digitalRead(PA8)); //blink led
  delay(200);
}
