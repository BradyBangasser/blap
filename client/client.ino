#include <stdio.h>

bool state = false;
void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    digitalWrite(LED_BUILTIN, state);
    state = !state;
    delay(2000);
}
