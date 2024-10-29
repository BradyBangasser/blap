#include "logging.h"
#include "domain_driver.h"
#include "signals.h"

#include <stdlib.h>

int main() {
    start_time();
    atexit(stop_time_print_data);
    
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = signal_handler;

    sigaction(SIGINT, &sa, NULL);

    if (init_device() == -1) {
        // cry about it
        ERROR("Socket initalization failed\n");
        return -1;
    }
    start_server();

    return 0;
}
