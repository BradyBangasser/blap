#pragma once

#include "constants.h"
#define SOCKET_PATH "/tmp/blap.socket"

#ifdef __cplusplus
extern "C" {
#endif

    int8_t start_server();
#ifdef __cplusplus
}
#endif
