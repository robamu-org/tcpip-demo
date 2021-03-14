#pragma once

#include "common.h"
#include <demo_config.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <vector>
#include <cstdint>

class ServerClass {
public:

    ServerClass(tcpip::DemoConfig& cfg);

    virtual~ ServerClass();

    int perform_operation();
private:
    tcpip::DemoModes mode;
    std::string server_port;
    std::string server_address;

#ifdef _WIN32
    SOCKET listen_socket = INVALID_SOCKET;
    SOCKET client_socket = INVALID_SOCKET;
#elif defined(__unix__)
    int listen_socket = 0;
    int client_socket = 0;
#endif

    std::vector<uint8_t> reception_buffer;

    int setup_server();
    int accept_connection();
    int perform_mode_operation();

    int perform_simple_echo_op();
};
