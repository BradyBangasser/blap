#include "blap.h"
#include "logging.h"
#include "domain_driver.h"

#include <stdlib.h>

uint8_t send_data(uint8_t *data, uint32_t len) { return -1; }
uint32_t recv_data(uint8_t *buffer, uint32_t len) { return -1; }

int main() {
    start_time();
    atexit(stop_time_print_data);
    if (init_device() == -1) {
        // cry about it
        ERROR("Socket initalization failed\n");
        return -1;
    }
    // start_server();

    return 0;
}
