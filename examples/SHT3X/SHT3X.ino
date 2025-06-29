#include <Arduino.h>
#include <Wire.h>
#include <ArtronShop_SHT3x.h>
#include <Rak3172_Canopus.h>

ArtronShop_SHT3x sht3x(0x44, &Wire); // ADDR: 0 => 0x44, ADDR: 1 => 0x45

void setup()
{
  Serial.begin(115200);
  Serial.print("\r\n************RAK3172_CANOPUS**************");
  init_io();
  enable_Vss3();
  disable_Vss5();
  disable_Vss12();

  Wire.begin();
  if (!sht3x.begin())
  {
    Serial.println("SHT3x not found !");
    delay(1000);
  }
}

void loop()
{
  if (sht3x.measure())
  {
    Serial.print("Temperature: ");
    Serial.print(sht3x.temperature(), 1);
    Serial.print(" *C\tHumidity: ");
    Serial.print(sht3x.humidity(), 1);
    Serial.print(" %RH");
    Serial.println();
  }
  else
  {
    sht3x.begin();
    Serial.println("SHT3x read error");
  }
  delay(1000);
}
