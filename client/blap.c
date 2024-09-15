#include "blap.h"

enum BLAP_ERROR_CODES send_blap_packet(uint8_t *data, uint16_t length) {
    if (data == NULL) return BLAP_POINTER_NOT_VALID;
    if (length > BLAP_MAX_PAYLOAD_LEN) return BLAP_TOO_LONG;

    struct blap_packet_header packet_header;
    memset(&packet_header, 0, sizeof(packet_header));

    packet_header.blap_id = BLAP_IDENTIFIER;
    packet_header.length = length;

    send_data((uint8_t *) &packet_header, length);
    return BLAP_NOT_IMPL;
}

enum BLAP_ERROR_CODES recv_blap_packet(uint8_t *data, uint16_t length) {
    uint8_t src_data[256] = {0};
    uint32_t data_len;
    data_len = recv_data(src_data, sizeof(src_data));

    if (data_len <= sizeof(struct blap_packet_header)) {
      return -1;
    }

    struct blap_packet_header *header = (struct blap_packet_header *) src_data;

    if (header->blap_id != BLAP_IDENTIFIER) {
      return src_data[0];
    }

    return data_len;
}
