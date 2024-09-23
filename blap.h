#pragma once
#include <inttypes.h>
#include <stddef.h>
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void blap_delay(uint32_t ms) {
    #ifdef Arduino_h
    delay(ms);
    #else
    struct timespec s, c;
    clock_gettime(CLOCK_MONOTONIC, &s);
    s.tv_sec += ms / 1000;
    s.tv_nsec += (ms % 1000) * 1e6;

    while (memcmp(&s, &c, sizeof(struct timespec)) >= 0) {
        clock_gettime(CLOCK_MONOTONIC, &c);
    }
    #endif
}

static const uint16_t BLAP_MAX_PAYLOAD_LEN = 69;
static const uint8_t BLAP_IDENTIFIER = 0x0C;

struct blap_context {
    const uint32_t addr;
    const bool server;
};

struct blap_context_opts {
    bool server;
};

struct blap_context *create_blap_context(struct blap_context_opts opts);
void free_blap_context(struct blap_context *context);

enum BLAP_ERROR_CODES {
    BLAP_NOT_IMPL = -1,
    BLAP_OK,
    BLAP_TOO_LONG,
    BLAP_POINTER_NOT_VALID,
};

struct blap_packet_header {
    uint8_t blap_id : 4;
    uint8_t packet_type : 4;
    uint32_t reserved_1;
    uint8_t frag;
    uint16_t length;
} __attribute__((packed));


enum BLAP_ERROR_CODES send_blap_packet(uint8_t *data, uint16_t length);
enum BLAP_ERROR_CODES recv_blap_packet(uint8_t *buffer, uint16_t length);

/**
 * Client must implement these
 */
uint8_t send_data(uint8_t *data, uint32_t len);
/**
 * receive data from device
 * @param buffer buffer for data
 * @param len max length of data
 * @returns length of received data
 */
uint32_t recv_data(uint8_t *buffer, uint32_t len);

#ifdef __cplusplus
}
#endif
