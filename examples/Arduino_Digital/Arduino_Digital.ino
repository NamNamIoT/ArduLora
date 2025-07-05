void setup()
{
    Serial.begin(115200);                      // Initialize serial communication at 115200 baud rate.
    Serial.println("Canopus Digital Example"); // Print a message indicating the start of the program.
    Serial.println("-----------------------"); // Print a separator line.
}

void loop()
{
    digitalWrite(PA8, HIGH); // Turn on the LED connected to pin LED_SEND.
    delay(1000);                  // Wait for 1 second.
    digitalWrite(PA8, LOW);  // Turn off the LED connected to pin LED_SEND.
    delay(1000);                  // Wait for 1 second before the next iteration of the loop.
}
