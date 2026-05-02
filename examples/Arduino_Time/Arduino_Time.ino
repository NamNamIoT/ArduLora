#include <ArduLora.h>
/***
 *  This example shows time function, including millis, micros, delay, delayMicroseconds.
***/

long delayTime = 1000;		// variable for setting the delay time 

void setup()
{
    // initialize serial communication at 115200 bits per second
    Serial.begin(115200);

    Serial.println("RAKwireless Arduino Time Example");
    Serial.println("------------------------------------------------------");
}

void loop()
{
    Serial.println("Now Time:");
    Serial.print("millis(): ");
    Serial.println(millis());	// show the time with millis
    Serial.print("micros(): ");
    Serial.println(micros());	// show the time with micros
  
    Serial.printf("After Delay %d milliseconds\n", delayTime);
    delay(delayTime);		// delay time (second)
    Serial.print("millis(): ");
    Serial.println(millis());	// show the time with millis
    Serial.print("micros(): ");
    Serial.println(micros());	// show the time with micros
  
    Serial.printf("After Delay %d microseconds\n", delayTime);
    delayMicroseconds(delayTime);	// delay time (Microseconds)
    Serial.print("millis(): ");
    Serial.println(millis());	// show the time with millis
    Serial.print("micros(): ");
    Serial.println(micros());	// show the time with micros
  
    Serial.println("");
  
    delayTime += 1000;		// delay time add 1000
  
    delay(5000);
}




