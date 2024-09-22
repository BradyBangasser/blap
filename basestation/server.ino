#include <SPI.h>
#include "blap.h"
#include "RF24.h"

#define CE_PIN D1
#define CSN_PIN D2

RF24 radio(CE_PIN, CSN_PIN);

uint8_t send_data(uint8_t *data, uint32_t len) {
  radio.write(data, len);
  return 0;
}

static inline void blink(uint32_t dur) {
  digitalWrite(LED_BUILTIN, LOW);
  delay(dur);
  digitalWrite(LED_BUILTIN, HIGH);
}

uint8_t address[][6] = { "1Node", "2Node" };
float payload = 0.0;

void setup() {

  Serial.begin(115200);
  while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT);

  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println("radio hardware is not responding!!");
    while (1) {}  // hold in infinite loop
  }

  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(address[0]);
  radio.stopListening();
  radio.enableDynamicPayloads();
  blink(200);
}  // setup

void loop() {
  char load[] = "loaded";
  send_blap_packet((uint8_t *) load, sizeof(load));
  delay(2000);
}  // loop
