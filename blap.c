#include "blap.h"

uint8_t send_blap_packet(uint8_t *data, uint16_t length) {
    if (data == NULL) return BLAP_POINTER_NOT_VALID;
    if (length > BLAP_MAX_PAYLOAD_LEN) return BLAP_TOO_LONG;



    return BLAP_NOT_IMPL;
}
