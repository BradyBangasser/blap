#include "blap.h"
#include <pthread.h>

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

enum BLAP_ERROR_CODES recv_blap(uint8_t *data, uint16_t length) {
    uint8_t *src_data;
    uint32_t data_len;
    data_len = recv_data(src_data, length);

    struct blap_packet_header *header = (struct blap_packet_header *) src_data;

    return BLAP_NOT_IMPL;
}

void *server_notification() {

    return NULL;
}

struct blap_context *create_blap_context(struct blap_context_opts opts) {
    struct blap_context *bc = (struct blap_context *) malloc(sizeof(struct blap_context));

    if (opts.server) {
        pthread_t tid;
        pthread_create(&tid, NULL, server_notification, NULL);
    }

    if (bc == NULL) {
        return NULL;
    }

    return bc;
}
