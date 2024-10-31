#define _GNU_SOURCE

#include "logging.h"
#include "domain_driver.h"
#include "handlers.h"

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
#include <stddef.h>

#define MAXCONNS 8
#ifdef DEBUG
#define __IF_DEBUG_ONCE(flag) if (has_debug_printed ^ flag) (has_debug_printed |= flag) &&
#else
#define __IF_DEBUG_ONCE(flag)
#endif

// TODO portablity

static int32_t sock;
static int8_t err = 0;
static struct pollfd poll_opts;
static struct pollfd sock_poll_opts[8];
static const int true_flag = 1;
static uint8_t conns = 0;
static struct connected_device *connected_devices[8] = {0};

#ifdef DEBUG
// This is to make sure debug messages only get printed once
enum debug_message {
    DM_EOM = 0x01, // Execute on Message   
    DM_NOM = 0x02, // No on Message defined
};
#endif

int8_t init_device() {
    DEBUG("Initializing Device\n");

    // TODO make this a memcpy function
    for (int i = 0; i < sizeof(sock_poll_opts) / sizeof(struct pollfd); i++) {
        sock_poll_opts[i].revents = POLLIN | POLLHUP;
        sock_poll_opts[i].events = POLLIN | POLLHUP;
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
    poll_opts.events = 0xFFFF;
    poll_opts.revents = 0xFFFF;

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
    socklen_t addr_len = 0;
    int new_sock;
    #ifdef DEBUG
    static uint8_t has_debug_printed = 0;
    #endif

    INFO("Starting Server\n");

    struct sockaddr client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    
    while (true) {
        int p = poll(sock_poll_opts, MAXCONNS, 1);

        if (p > 0) {
            for (int i = 0; i < MAXCONNS; i++) {
                if (sock_poll_opts[i].revents & POLLHUP) {
                    // client has disconnected, free mem and make sure socket spot is available

                    if (on_client_disconnect != NULL) {
                        DEBUGF("Executing on_client_disconnect at 0x%lx\n", (uint64_t) on_client_disconnect);
                        on_client_disconnect(connected_devices[i]);
                    } else {
                        WARN("No on_disconnect function defined\n");
                    }


                    INFOF("Resetting connection no %d\n", i);
                    close(sock_poll_opts[i].fd);
                    sock_poll_opts[i].fd = -1;
                    reset_connection(i);
                    free(connected_devices[i]);
                    connected_devices[i] = 0;
                    continue;
                }

                if (sock_poll_opts[i].revents & POLLIN) {
                    uint8_t buffer[512] = {0};

                    int32_t len = recv(connected_devices[i]->addr, buffer, sizeof(buffer), 0);
                    
                    if (on_client_message != NULL) {
                        __IF_DEBUG_ONCE(DM_EOM) DEBUGF("Executing on_client_message at 0x%lx\n", (uint64_t) on_client_message);
                        struct message *msg = malloc(sizeof(struct message));

                        if (msg == NULL) {
                            ERRORF("Failed to malloc data for struct message, errno: %d\n", errno);
                            continue;
                        }

                        msg->length = len;
                        msg->data = malloc(sizeof(uint8_t) * len);

                        if (msg->data == NULL) {
                            free(msg);
                            msg = 0;
                            ERRORF("Failed to malloc data for struct message, errno: %d\n", errno);
                            continue;
                        }

                        memcpy(msg->data, buffer, len);

                        on_client_message(connected_devices[i], msg);
                    } else {
                        WARN("No on_client_message defined\n");
                    }
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

        connected_devices[cidx] = malloc(sizeof(struct connected_device));
        connected_devices[cidx]->addr = new_sock;
        connected_devices[cidx]->connection_id = cidx;

        if (err == -1) {
            ERRORF("Failed to write to socket %d, errno %d\n", new_sock, errno);
            return -1;
        }

        if (recv_handshake(connected_devices[cidx]) != 0) {
            ERROR("Handshake failed\n");
            close_connection(cidx);
            continue;
        }

        INFOF("Accepted connection no %d\n", cidx);
        
        if (on_connect != NULL) {
            DEBUGF("Executing on_connect at 0x%lx\n", (uint64_t) on_connect);
            on_connect(connected_devices[cidx]);
        }
    }
}

int8_t pconnect() {
    int sock = socket(AF_LOCAL, SOCK_STREAM, 0);
    struct sockaddr_un addr;
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, SOCKET_PATH);

    int err = connect(sock, (struct sockaddr *) &addr, sizeof(addr));
    
    if (err == -1) {
        ERRORF("Error connecting to socket, errno: %d\n", errno);
        return -1;
    }

    int cidx = first_available_connection();

    connected_devices[cidx] = malloc(sizeof(struct connected_device));

    if (connected_devices[cidx] == NULL) {
        ERRORF("Failed to allocate memory for connected_device, errno %d\n", errno);
        return -1;
    }

    connected_devices[cidx]->addr = sock;
    connected_devices[cidx]->connection_id = cidx;
    set_connection(cidx);

    DEBUG("Starting Handshake\n");
    if (start_handshake(connected_devices[cidx])) {
        ERROR("Handshake failed\n");
        close_connection(cidx);
        return -2;
    }
    DEBUG("Handshake success\n");

    return cidx;
}

const struct connected_device * const get_connection(uint8_t connection_id) {
    if (connection_id >= MAXCONNS) {
        return NULL;
    }

    return connected_devices[connection_id];
}

int8_t start_client(void (*cb)()) {
    int i;
    ssize_t len;
    uint8_t buffer[256];
    #ifdef DEBUG
    static uint8_t has_debug_printed = 0;
    #endif

    while (true) {
        if (poll(sock_poll_opts, MAXCONNS, 1) > 0) {
            for (i = 0; i < MAXCONNS; i++) {
                if (sock_poll_opts[i].revents & POLLHUP) {
                   if (on_disconnect != NULL) {
                        on_disconnect(connected_devices[i]);
                    } else {
                        WARN("No on_disconnect function defined\n");
                    }

                    INFOF("Resetting connection no %d\n", i);
                    close(sock_poll_opts[i].fd);
                    sock_poll_opts[i].fd = -1;
                    reset_connection(i);
                    free(connected_devices[i]);
                    connected_devices[i] = 0;
                }

                if (sock_poll_opts[i].revents & POLLIN) {
                    len = recv(connected_devices[i]->addr, buffer, sizeof(buffer), 0);

                    if (len == -1) {
                        if (errno == EWOULDBLOCK) {
                            WARN("Poll reported message, but recv didn't\n");
                            continue;
                        }

                        ERRORF("recv failed, errno: 0x%x\n", errno);
                        continue;
                    }

                    if (on_message != NULL) {
                        struct message *msg = malloc(sizeof(struct message));

                        if (msg == NULL) {
                            ERRORF("Failed to malloc data for struct message, errno: %d\n", errno);
                            continue;
                        }

                        msg->length = len;
                        msg->data = malloc(sizeof(uint8_t) * len);

                        if (msg->data == NULL) {
                            free(msg);
                            msg = 0;
                            ERRORF("Failed to malloc data for struct message, errno: %d\n", errno);
                            continue;
                        }

                        memcpy(msg->data, buffer, len);
                        #ifdef DEBUG
                        if (has_debug_printed & DM_EOM)
                            (has_debug_printed |= DM_EOM) &&
                        #endif
                                DEBUGF("Executing on_message at 0x%lx\n", (uint64_t) on_message);

                        on_message(connected_devices[i], msg);
                    } else {
                        WARN("No on_message function defined\n");
                    }
                }

                // Handle other things
            }
        }

        if (cb != NULL) {
            __IF_DEBUG_ONCE(DM_EOM) DEBUGF("Executing client callback at 0x%lx\n", (uint64_t) cb);
            cb();
        } else {
            WARN("No client callback defined, this is likely not intentional\n");
        }
    }
}

__ssize_t recv_data(const struct connected_device * const dev, uint8_t *buffer, uint32_t length, int32_t timeout) {
    __ssize_t l;
    struct timespec ts, cts;
    uint8_t *tbuf = malloc((sizeof(struct message) - sizeof(uint8_t *) + length) * sizeof(char));

    if (tbuf == NULL) {
        return -3;
    }

    if (timeout > 0) {
        clock_gettime(CLOCK_MONOTONIC, &ts);

        ts.tv_sec += timeout / 1000;
        ts.tv_nsec += (timeout % 1000) * (uint) 1e6;
    }

    while ((l = recv(dev->addr, tbuf, length, 0)) <= 0) {
        if (errno != EWOULDBLOCK) {
            ERRORF("Error receiving data: %d\n", errno);
            return -1;
        }

        if (timeout > 0) {
            clock_gettime(CLOCK_MONOTONIC, &cts);
            if (cts.tv_sec >= ts.tv_sec && cts.tv_nsec >= ts.tv_nsec) {
                return -2;
            }
        }
    }

    memcpy(buffer, tbuf + offsetof(struct message, data), ((struct message *) tbuf)->length);
    free(tbuf);
    
    return l;
}

__ssize_t send_data_to(const struct connected_device* const dev, uint8_t *data, uint32_t len) {
    // ensure connection is valid

    if (dev == NULL || data == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (len == 0) {
        WARNF("Sending packet of length 0 to 0x%lx\n", dev->addr);
    }

    return write(dev->addr, data, len);
}

__ssize_t send_messages_to(const struct connected_device * const dev, const struct message * messages, uint32_t nmess) {
    uint32_t i = 0, full_packet_len;
    __ssize_t result, total = 0;
    uint8_t *data = NULL;

    if (nmess > 1) {
        WARN("Frag Packets not yet supported\n");
    }

    if (dev == NULL || messages == NULL) {
        return -1;
    }

    while (i < 1) { // This will need to be changed to nmess in the future
        full_packet_len = (sizeof(struct message) - sizeof(uint8_t *) + messages[i].length) * sizeof(char);
        data = malloc(full_packet_len);

        DEBUGF("Packet Length: %d\n", full_packet_len);
        if (data == NULL) {
            return -1;
        }

        memcpy(data, messages + i, offsetof(struct message, data));
        memcpy(data + offsetof(struct message, data), messages[i].data, messages[i].length);

        result = send_data_to(dev, data, full_packet_len);

        DEBUGF("RESULT: %li\n", result);
        free(data);
        data = NULL;

        if (result < 0) {
            ERRORF("Failed to send data, errno: %d\n", errno);
            return result;
        }

        total += result;
        i++;
    }

    return total;
}

uint8_t close_connection(uint8_t cidx) {
    // send disconnect packet
    close(connected_devices[cidx]->addr);
    free(connected_devices[cidx]);
    connected_devices[cidx] = 0;

    return 0;
}
