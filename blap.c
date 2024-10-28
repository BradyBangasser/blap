#include "handlers.h"
#include "logging.h"

#include <unistd.h>
#include <stdlib.h>

enum packet_types {
    PT_SUP,
};

struct blap_packet {
    uint32_t length;
};

uint8_t create_packet(enum packet_types type, uint8_t *payload, struct message **packet, uint32_t *length) {

    if (packet == NULL || (payload == NULL && type != PT_SUP) || length == NULL) {
        return 2;
    }

    switch (type) {
        case PT_SUP: {
                         static const char sup[] = "sup";
                         struct message *m = *packet;
                         m = (struct message *) calloc(1, sizeof(struct message));

                         if (m == NULL) {
                             return 1;
                         }

                         m->data = malloc(sizeof(sup));

                         if (m->data == NULL) {
                             free(m);
                             m = 0;
                             return 1;
                         }

                         memcpy(m->data, sup, sizeof(sup));
                         *length = 1;
                         return 0;
        }
    }

    return 1;
}

// Called by client
/**
 *
 * @returns 0 on success
 */
uint8_t start_handshake(const struct connected_device * const dev) {
    send_data_to(dev->addr);
    return 1;
}

void on_connect(struct connected_device *const connection) {
    const char *message = "Hello There!";
    INFOF("Connected to device, connection id: %d, device address: %ld\n", connection->connection_id, connection->addr);
    send_data_to(connection, (uint8_t *) message, strlen(message));
}

void on_disconnect(struct connected_device *const connection) {
    INFOF("Disconnected from device, connection id: %d, device address %ld\n", connection->connection_id, connection->addr);
}

void on_client_message(const struct connected_device * const src, struct message * const msg) {
    char message[] = "From the Server";
    write(src->addr, message, sizeof(message));
}
