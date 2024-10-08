#define _GNU_SOURCE

#include "blap.h"
#include "logging.h"
#include "domain_driver.h"

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>

#define MAXCONNS 8

// TODO portablity

static int32_t sock;
static int8_t err = 0;
static struct pollfd poll_opts;
static struct pollfd sock_poll_opts[8];
static const int true_flag = 1;
static uint8_t conns = 0;
static struct connection *connections[MAXCONNS] = { 0 };

int8_t init_device() {
    DEBUG("Initializing Device\n");

    // TODO make this a memcpy function
    for (int i = 0; i < sizeof(sock_poll_opts) / sizeof(struct pollfd); i++) {
        sock_poll_opts[i].revents = POLLRDHUP;
    }

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
    strcpy(addr.sun_path, SOCKET_PATH);

    if (access(SOCKET_PATH, F_OK) == 0) {
        WARNF("Removing existing socket '%s'\n", SOCKET_PATH);
        err = unlink(SOCKET_PATH);

        if (err == -1) {
            WARNF("Failed to remove socket '%s', this may cause an ADDRINUSE error in the future\n", SOCKET_PATH);
            err = 0;
        }
    }

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
    poll_opts.events = POLLRDHUP;
    poll_opts.revents = 0;

    DEBUG("Initialized device\n");
    return 0;
}

int8_t first_available_connection() {
    for (int i = 0; i < sizeof(conns) * 8; i++) {
        if ((conns & (1 << i)) == 0) {
            return i;
        }
    }

    return -1;
}

void set_connection(uint8_t i) {
    conns |= (1 << i);
}

void reset_connection(uint8_t i) {
    conns &= (~(1 << i));
}

int8_t start_server() {
    socklen_t addr_len;
    int new_sock;
    INFO("Starting Server\n");

    struct sockaddr client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    addr_len = addr_len;
    
    while (true) {
        int p = poll(sock_poll_opts, MAXCONNS, 1);

        if (p > 0) {
            for (int i = 0; i < MAXCONNS; i++) {
                if (sock_poll_opts[i].revents & POLLHUP) {
                    // client has disconnected, free mem and make sure socket spot is available
                    INFOF("Resetting connection no %d\n", i);
                    close(sock_poll_opts[i].fd);
                    sock_poll_opts[i].fd = -1;
                    reset_connection(i);
                }
            }
        }
                    

        new_sock = accept(sock, &client_addr, &addr_len);

        if (new_sock < 0) {
            if (errno == EAGAIN) continue;
            ERRORF("Failed to accept connection, errno: 0x%x\n", errno);
            return -1;
        }

        if (conns == 0xFF) {
            WARNF("Max amount of connections reached (%ul)\n", (unsigned int) sizeof(conns) * 8);
            // TODO delay
            close(new_sock);
            continue;
        }

        uint8_t cidx = first_available_connection();

        set_connection(cidx);
        sock_poll_opts[cidx].fd = new_sock;

        INFOF("Accepted connection no %d\n", cidx);
        const char *message = "Hello World";
        err = write(new_sock, message, strlen(message));

        if (err == -1) {
            ERRORF("Failed to write to socket %d, errno %d\n", new_sock, errno);
            return -1;
        }
    }
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


