#pragma once

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

    socket_t listen_socket = INVALID_SOCKET;
    socket_t client_socket = INVALID_SOCKET;

    int setup_server();
    int setup(struct addrinfo& hints) override;

    int accept_connection();
    int perform_mode_operation();

    int perform_mode_1_echo();
    int perform_mode_2();
    int perform_mode_3();

    int perform_shutdown();
};
