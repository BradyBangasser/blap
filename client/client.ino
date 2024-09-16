#include <SPI.h>
#include "blap.h"
#include "RF24.h"

#define CE_PIN D1
#define CSN_PIN D2
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);

uint8_t address[][6] = { "1Node", "2Node" };
char payload[256] = {0};

static inline void blink(uint32_t dur) {
  digitalWrite(LED_BUILTIN, LOW);
  delay(dur);
  digitalWrite(LED_BUILTIN, HIGH);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }

  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println("radio hardware is not responding!!");
    while (1) {}  // hold in infinite loop
  }

  radio.setPALevel(RF24_PA_MAX);
  // radio.setPayloadSize(sizeof(payload));
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1, address[0]);  // using pipe 1
  radio.startListening();  // put radio in RX mode
}

void loop() {
    uint8_t pipe;
    if (radio.available(&pipe)) {
      uint8_t bytes = radio.getDynamicPayloadSize();
      radio.read(payload, bytes);
      Serial.printf("0x%x\n", payload[0]);
      blink(300);
    }
}  // loop