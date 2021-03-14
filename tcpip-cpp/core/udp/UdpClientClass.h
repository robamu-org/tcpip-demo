#pragma once

#include <core/TcpipBase.h>

class UdpClientClass: public TcpipBase {
public:
    UdpClientClass(tcpip::DemoConfig& cfg, size_t rec_buf_size);

    UdpClientClass() = delete;

    virtual~ UdpClientClass();

    int perform_operation() override;

private:

    socket_t server_socket = INVALID_SOCKET;

    struct addrinfo* server_addr = nullptr;

    int setup(struct addrinfo& hints) override;

    int setup_client();

    int perform_send_operation();
};
