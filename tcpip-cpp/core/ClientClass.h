#pragma once

#include <core/TcpipBase.h>
#include <demo_config.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <vector>

class ClientClass: public TcpipBase {
public:

    ClientClass(tcpip::DemoConfig& cfg, size_t reception_buf_size);

    ClientClass() = delete;

    virtual~ ClientClass();

    int perform_operation() override;

private:

#ifdef _WIN32
    SOCKET connect_socket = INVALID_SOCKET;
#else
    int connect_socket = 0;
#endif
    int setup(struct addrinfo& hints) override;

    int attempt_connection();
    int perform_send_operation();
    int perform_recv_operation();

    int perform_simple_send_op();
    int perform_echo_recv_operation();
};
