#include <Rak3172_Canopus.h>
#include <TinyGPSPlus.h>
TinyGPSPlus gps; //GPS ATGM336H
void setup() {
  
  Serial.begin(115200);
  Serial1.begin(9600);
  init_io();
  enable_Vss3();
  while (Serial1.available()) {
    Serial1.read();
  }
}

void loop() {
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());
  }
  float gps_lat = gps.location.lat();
  float gps_lng = gps.location.lng();
  float gps_speed = gps.speed.mps();
}
