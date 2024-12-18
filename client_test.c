#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdlib.h>

#include "logging.h"
#include "handlers.h"

static int c = 1;

void cb() {
    static int8_t connection_id = -1;

    if (c) {
        c = 0;
        connection_id = pconnect();

        if (connection_id < 0) {
            ERRORF("Failed to connect, error: %d\n", connection_id);
            exit(1);
        }
    }
}

int main() {
    INFO("Starting Client\n");
    start_client(cb);
    
//    int sock = socket(AF_LOCAL, SOCK_STREAM, 0);
//
//    if (sock < 0) {
//        ERRORF("Failed to create socket, errno: %d\n", errno);
//        return -1;
//    }
//
//    struct sockaddr_un addr;
//    addr.sun_family = AF_LOCAL;
//    strcpy(addr.sun_path, SOCKET_PATH);
//
//    int err = connect(sock, (struct sockaddr *) &addr, sizeof(addr));
//    
//    if (err == -1) {
//        ERRORF("Error connecting to socket, errno: %d\n", errno);
//        return -1;
//    }
//
//    char buffer[256];
//    recv(sock, buffer, sizeof(buffer), 0);
//    INFOF("received %s\n", buffer);

    while (1);
    return 0;
}

