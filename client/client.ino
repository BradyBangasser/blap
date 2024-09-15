#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "blap.h"

RF24 radio(D1, D2);

uint32_t recv_data(uint8_t *buffer, uint32_t len) {
  uint32_t payloadLength = 0;
  if (radio.available()) {
    radio.read((void *) buffer, len);
    Serial.printf("0x%x\n", buffer[0]);
  }

  return payloadLength;
}

uint8_t send_data(uint8_t *data, uint32_t len) { return -1; }

bool state = false;
uint8_t payload = 0;
const uint8_t pipe[] = { 0xCC, 0xCE, 0xCC, 0xCE, 0xCC };
uint32_t data_len;

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
  uint8_t buffer[256] = {0};
  if (radio.available()) {
    radio.read((void *) buffer, 256);
    Serial.printf("0x%x\n", buffer[0]);
  }
  // data_len = recv_blap_packet(NULL, 256);
  // if (data_len != -1) {
  //   Serial.printf("len: %d\n", data_len);
  //   blink();
  // }

  delay(50);
}
