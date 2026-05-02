#include <ArduLora.h>
/***
 *  This example shows Serial lock function, the Serial locks until receiving correct password or exceed 30 seconds.
***/

unsigned long start, now, elapsed;
bool locked;

void setup()
{
    Serial.begin(115200, RAK_AT_MODE);
    /* Wait 5s for USB to be plugged in, or the log messages will be gone */
  
    Serial.println("RAKwireless System Serial Lock Example");
    Serial.println("------------------------------------------------------");
    /* Lock serial port with password => 12345678 */
    String password = "12345678";
    Serial.println("Type 12345678 and then press enter to unlock the serial port:");
    Serial.println("(If the password is not correct, the serial port will be auto unlocked after 30s..)");
  
    bool ret;
    if (!(ret = api.system.pword.set(password))) {
        Serial.printf("System serial Lock - set password parameter is incorrect! \r\n");
        return;
    }
    api.system.pword.lock();
    locked = true;
    start = millis();
}

void loop()
{
    now = millis();
    if ((elapsed = (now - start)) > 30000 && locked == true) {
        Serial.println("Serial port is auto unlocked now..\r\n");
        api.system.pword.unlock();
        locked = false;
    }
    Serial.println("=========");
    Serial.printf("start=%lu\r\n", start);
    Serial.printf("now=%lu\r\n", now);
    Serial.printf("elapsed time=%lu\r\n", elapsed);
    delay(1000);
}




