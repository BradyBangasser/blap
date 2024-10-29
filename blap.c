#include "handlers.h"
#include "logging.h"

#include <unistd.h>
#include <stdlib.h>

enum packet_types {
    PT_SUP,
    PT_ESUP,
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
                         *packet = (struct message *) calloc(1, sizeof(struct message));

                         if (*packet == NULL) {
                             return 1;
                         }

                         (*packet)->data = malloc(sizeof(sup));

                         if ((*packet)->data == NULL) {
                             free(*packet);
                             *packet = 0;
                             return 1;
                         }

                         memcpy((*packet)->data, sup, sizeof(sup));
                         (*packet)->length = sizeof(sup);
                         *length = 1;
                         return 0;
                     }
        case PT_ESUP: {
                          static const char esup[] = "esup";
                          *packet = (struct message *) calloc(1, sizeof(struct message));

                          if (*packet == NULL) {
                              return 1;
                          }

                          (*packet)->data = malloc(sizeof(esup));

                          if ((*packet)->data == NULL) {
                              free(*packet);
                              *packet = 0;
                              return 1;
                          }

                          memcpy((*packet)->data, esup, sizeof(esup));
                          (*packet)->length = sizeof(esup);
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
    struct message *msgs = NULL;
    uint8_t buffer[512] = { 0 };
    uint32_t len;

    if (create_packet(PT_SUP, NULL, &msgs, &len) != 0) {
        return 1;
    }

    send_messages_to(dev, msgs, 1);
    
    if (recv_data(dev, buffer, sizeof(buffer), -1) <= 0 || strcmp((char *) buffer, "sup") != 0) {
        return 2;
    }

    // send ESUP packets
    if (create_packet(PT_ESUP, NULL, &msgs, &len) != 0) {
        return 4;
    }

    send_messages_to(dev, msgs, 1);

    if (recv_data(dev, buffer, sizeof(buffer), -1) <= 0 || strcmp((char *) buffer, "esup") != 0) {
        return 5;
    }
    
    return 0;
}

uint8_t recv_handshake(const struct connected_device * const dev) {
    uint8_t buffer[512];
    __ssize_t len;
    uint32_t ilen;
    struct message *msgs = 0;

    // expect sup
    // TODO, create timeouts
    len = recv_data(dev, buffer, sizeof(buffer), -1);

    if (len <= 0) {
        return 1;
    }

    if (strcmp((char *) buffer, "sup") != 0) {
        return 2;
    }

    if (create_packet(PT_SUP, NULL, &msgs, &ilen) != 0) {
        return 3;
    }

    send_messages_to(dev, msgs, 1);

    return 0;
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
    DEBUGF("%s\n", msg->data);
    ERROR("HERE\n");
    char message[] = "From the Server";
    write(src->addr, message, sizeof(message));
}
