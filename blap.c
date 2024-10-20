#include "handlers.h"
#include "logging.h"

const char *message = "Hello There!";

void on_connect(struct connected_device *const connection) {
    INFOF("Connected to device, connection id: %d, device address: %ld\n", connection->connection_id, connection->addr);
    send_data_to(connection, (uint8_t *) message, strlen(message));
}

void on_disconnect(struct connected_device *const connection) {
    INFOF("Disconnected from device, connection id: %d, device address %ld\n", connection->connection_id, connection->addr);
}
