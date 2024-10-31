#pragma once
#include <stdint.h>

struct connected_device {
    uint64_t addr;
    uint32_t connection_id;
};

// TODO, add buffering to this to make it more efficient
struct message {
    uint8_t packet_type;
    uint32_t length;
    uint8_t *data;
}__attribute__((packed));
