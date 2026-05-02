#include <ArduLora.h>
uint8_t ledPin = PA8;
uint8_t inputPin = 0xFF;

int buttonState;

rt rtBlink;
rt rtButton;

int blinkThread(struct rt *rt) {
    RT_BEGIN(rt);

    for (;;) {
        if (buttonState == HIGH) {
            digitalWrite(ledPin, HIGH);
            RT_SLEEP(rt, 200);
            digitalWrite(ledPin, LOW);
            RT_SLEEP(rt, 100);
        } else {
            digitalWrite(ledPin, LOW);
            RT_YIELD(rt);
        }
    }

    RT_END(rt);
}

int buttonThread(struct rt *rt) {
    RT_BEGIN(rt);
    for (;;) {
        buttonState = digitalRead(inputPin);
        RT_YIELD(rt);
    }
    RT_END(rt);
}

void setup()
{
    Serial.begin(115200);

    Serial.println("RAK3172_Canopus Thread Example");
    Serial.println("------------------------------------------------------");
    pinMode(ledPin, OUTPUT);
    pinMode(inputPin, INPUT);
    buttonState = digitalRead(inputPin);
    RT_INIT(&rtBlink);
    RT_INIT(&rtButton);
}

void loop()
{
    RT_SCHEDULE(blinkThread(&rtBlink));
    RT_SCHEDULE(buttonThread(&rtButton));
}




