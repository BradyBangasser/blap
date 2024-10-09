#pragma once

#include "logging.h"
#include <signal.h>
#include <stdlib.h>

void signal_handler(int sig) {
    if (sig == SIGSEGV) {
        ERROR("Segfault, 10 pushups\n");
    }

    exit(sig);
}
