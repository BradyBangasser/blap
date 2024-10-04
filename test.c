#include "blap.h"
#include "logging.h"

uint8_t send_data(uint8_t *data, uint32_t len) { return -1; }
uint32_t recv_data(uint8_t *buffer, uint32_t len) { return -1; }

int main() {
    int d;
    if (init_device() == -1) {
        // cry about it
        ERROR("Socket initalization failed\n");
        return -1;
    }

    while (1) {
        d = data_available();
        INFOF("Data Available %d\n", d);
    }

    return 0;
}
