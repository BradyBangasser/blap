#include "blap.h"
#include "logging.h"
#include "domain_driver.h"

#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/un.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


// TODO portablity

static int32_t sock;
static int8_t err = 0;
static struct pollfd poll_opts;
static const int true_flag = 1;

struct ucred {
    uint32_t pid;
    uint32_t uid;
    uint32_t gid;
};

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
    poll_opts.events = POLLIN;
    poll_opts.revents = POLLIN;

    DEBUG("Initialized device\n");
    return 0;
}

pid_t get_pid(int sock) {
    struct ucred cred;
    socklen_t cred_len = sizeof(cred);

    getsockopt(sock, SOCK_STREAM, SO_PEERPIDFD, &cred, &cred_len);

    return cred.gid;
}

int8_t start_server() {
    socklen_t addr_len;
    int new_sock;
    INFO("Starting Server\n");

    struct sockaddr client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    addr_len = addr_len;
    
    while (true) {
        new_sock = accept(sock, &client_addr, &addr_len);

        if (new_sock < 0) {
            if (errno == EAGAIN) continue;
            ERRORF("Failed to accept connection, errno: 0x%x\n", errno);
            return -1;
        }

        pid_t pid = get_pid(new_sock);

        INFOF("Accepted connection from %d\n", pid);
        return 1;
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


