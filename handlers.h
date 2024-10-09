#pragma once

// ----- Server Handlers -----
__attribute__((weak)) void on_client_connect();
__attribute__((weak)) void on_client_disconnect();
__attribute__((weak)) void on_unexpected_client_disconnect();
__attribute__((weak)) void on_client_message();

// ----- Client Handlers -----
__attribute__((weak)) void on_connect();
__attribute__((weak)) void on_disconnect();
__attribute__((weak)) void on_unexpected_disconnect();
__attribute__((weak)) void on_message();
