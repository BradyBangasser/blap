#include <sys/socket.h>
#include <errno.h>
#include <sys/un.h>

#include "logging.h"
#include "handlers.h"

uint8_t send_data(uint8_t *data, uint32_t len) { return -1; }
uint32_t recv_data(uint8_t *buffer, uint32_t len) { return -1; }

int main() {
    INFO("Starting Client\n");
    start_client(NULL);
    
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

