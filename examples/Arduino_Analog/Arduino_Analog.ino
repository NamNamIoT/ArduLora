void setup()
{
  Serial.begin(115200);                                                     // Initialize serial communication at 115200 baud rate.
  Serial.println("Canopus Analog Example");                                 // Print a message indicating the start of the program.
  Serial.println("------------------------------------------------------"); // Print a separator line.
  analogReadResolution(12); // Set analog read resolution to 12 bits.
}

void loop()
{
  float AI1 = analogRead(PB4) * 2.58;  // Read analog voltage from pin AI1_PIN and store it in AI1.
  Serial.printf("AI1 = %0.0fmV\r\n", AI1); // Print the analog voltage value of AI1 in millivolts.

  float AI2 = analogRead(PA10) * 2.58;  // Read analog voltage from pin AI2_PIN and store it in AI2.
  Serial.printf("AI2 = %0.0fmV\r\n", AI2); // Print the analog voltage value of AI2 in millivolts.

  delay(1000); // Wait for 1 second before the next iteration of the loop.
}
