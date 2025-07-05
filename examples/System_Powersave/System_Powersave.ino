void setup() {
  Serial.begin(115200);
  Serial.println("RAKwireless System Powersave Example");
  Serial.println("------------------------------------------------------");
}

void loop() {
  Serial.print("The timestamp before sleeping: ");
  Serial.print(millis());
  Serial.println(" ms");
  Serial.println("(Wait 30 seconds or Press any key to wakeup)");
  api.system.sleep.all(30000);
  Serial.print("The timestamp after sleeping: ");
  Serial.print(millis());
  Serial.println(" ms");
}
