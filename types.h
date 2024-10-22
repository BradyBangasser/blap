#pragma once
#include <stdint.h>

struct connected_device {
    uint64_t addr;
    uint32_t connection_id;
};

// TODO, add buffering to this to make it more efficient
struct message {
    uint8_t *data;
    uint32_t length;
};
