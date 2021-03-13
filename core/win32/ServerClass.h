#pragma once

#include "common.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <vector>
#include <cstdint>

class ServerClass {
public:

    ServerClass(tcpip::Modes mode);

    virtual~ ServerClass();

    int perform_operation();
private:
    tcpip::Modes mode;

    SOCKET listen_socket = INVALID_SOCKET;
    SOCKET client_socket = INVALID_SOCKET;

    std::vector<uint8_t> reception_buffer;
    size_t reception_buffer_len;

    int setup_server();
    int accept_connection();
    int perform_mode_operation();

    int perform_simple_echo_op();
};
