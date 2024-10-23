#pragma once

#include <stdint.h>
#include "types.h"

// ----- Server Handlers -----
__attribute__((weak)) void on_client_connect(struct connected_device * const);
__attribute__((weak)) void on_client_disconnect(struct connected_device * const);
__attribute__((weak)) void on_unexpected_client_disconnect();
__attribute__((weak)) void on_client_message();

// ----- Client Handlers -----
__attribute__((weak)) void on_connect(struct connected_device * const);
__attribute__((weak)) void on_disconnect(struct connected_device * const);
__attribute__((weak)) void on_unexpected_disconnect();
__attribute__((weak)) void on_message(const struct connected_device * const, struct message * const);

// Client must implement this functions:
void on_data(void (*)(uint8_t *data, uint32_t len));
__ssize_t send_data_to(const struct connected_device* const dev, uint8_t *data, uint32_t len);
// On success returns connection id
int8_t pconnect();
int8_t start_client(void (*cb)());
