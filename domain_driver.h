#pragma once

#include "constants.h"
#define SOCKET_PATH "/tmp/blap.socket"

#ifdef __cplusplus
extern "C" {
#endif

    struct connection { 
        int fd;
    };

    int8_t start_server();
    int8_t init_device();
#ifdef __cplusplus
}
#endif
