#include "blap.h"
#include "logging.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/un.h>
#include <sys/socket.h>

// TODO portablity

static int32_t sock;
static int8_t err = 0;
static struct pollfd poll_opts;
static const int true_flag = 1;

int8_t init_device() {
    DEBUG("Initializing Device\n");
    sock = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sock < 0) {
        ERRORF("Error initializing socket, errno: %d\n", errno);
        return -1;
    }

    err = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &true_flag, sizeof(true_flag));
    if (err < 0) {
        ERRORF("Failed to set socket address reuse, errno: %d\n", errno);
        return -1;
    }

    err = fcntl(sock, F_SETFL, O_NONBLOCK);
    if (err != 0) {
        ERRORF("Error setting socket options, errno %d\n", errno);
        return -1;
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, "/var/run/blap.socke");
    err = bind(sock, (struct sockaddr *) &addr, sizeof(addr));
    if (err != 0) {
        ERRORF("Error binding socket, errno %d\n", errno);
        return -1;
    }

    err = listen(sock, 10);
    if (err == -1) {
        ERRORF("Error listening on socket, errno %d\n", errno);
        return -1;
    }

    poll_opts.fd = sock;
    poll_opts.events = POLLIN;
    poll_opts.revents = POLLIN;

    DEBUG("Initialized device\n");
    return 0;
}

int8_t data_available() {
    DEBUG("Polling socket for available messages\n");
    int p = poll(&poll_opts, 1, 10);
    if (p == -1) {
        ERRORF("Failed to poll socket, errno %d\n", errno);
        return -1;
    }

    DEBUG("Polled socket\n");
    return !!p;
}


