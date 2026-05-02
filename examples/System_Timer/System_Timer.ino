#include <ArduLora.h>
void handler(void *data)
{
    Serial.printf("[%lu]This is the handler of timer #%d\r\n", millis(), (int)data);
    /* Actually, the handler is not executed in interrupt context. The real ISR for timer just sends an event to the system event queue.
     * If main loop found there is any event in the event queue, the handler for that event will be processed. */
}

void setup()
{
    Serial.begin(115200);
  
    Serial.println("RAKwireless System Timer Example");
    Serial.println("------------------------------------------------------");
  
    for (int i = 0 ; i < RAK_TIMER_ID_MAX ; i++) {
        if (api.system.timer.create((RAK_TIMER_ID)i, (RAK_TIMER_HANDLER)handler, RAK_TIMER_PERIODIC) != true) {
            Serial.printf("Creating timer #%d failed.\r\n", i);
            continue;
        }
        if (api.system.timer.start((RAK_TIMER_ID)i, (i+1)*1000, (void *)i) != true) {
            Serial.printf("Starting timer #%d failed.\r\n", i);
            continue;
        }
    }
}

void loop()
{
}




