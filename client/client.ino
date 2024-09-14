#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

bool state = false;

RF24 radio(D1, D2);
uint8_t payload = 0;

const uint64_t pipe = 0xE8E8F0F0E1LL;

inline void blink() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(150);
  digitalWrite(LED_BUILTIN, HIGH);
}

void setup() {
  Serial.begin(115200);

  while (!Serial) blink();
  Serial.print("\n\nHERE\n");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  if (!radio.begin()) {
    Serial.print("Failed to connect to radio\n");
    while (1) {
      blink();
      delay(200);
    }
  }

  radio.setChannel(115);
  radio.openReadingPipe(0, pipe);
  radio.startListening();
  Serial.print("Init\n");
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  if (radio.available()) {
    radio.read(&payload, 1);
    Serial.printf("Payload Received: 0x%x\n", payload);
    blink();
  } else {
    // Serial.print("Nothing\n");
  }

  delay(50);
}
