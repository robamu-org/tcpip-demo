#pragma once

#include "common.h"
#include <core/TcpipBase.h>
#include <demo_config.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <vector>
#include <cstdint>

class TcpServerClass: public TcpipBase {
public:

    TcpServerClass(tcpip::DemoConfig& cfg, size_t reception_buf_size);

    TcpServerClass() = delete;

    virtual~ TcpServerClass();

    int perform_operation() override;
private:

#ifdef _WIN32
    SOCKET listen_socket = INVALID_SOCKET;
    SOCKET client_socket = INVALID_SOCKET;
#elif defined(__unix__)
    int listen_socket = 0;
    int client_socket = 0;
#endif
    int setup(struct addrinfo& hints) override;

    int setup_server();
    int accept_connection();
    int perform_mode_operation();

    int perform_simple_echo_op();
};
