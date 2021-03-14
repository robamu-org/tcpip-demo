#pragma once

#include <core/TcpipBase.h>
#include <demo_config.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <vector>

class TcpClientClass: public TcpipBase {
public:

    TcpClientClass(tcpip::DemoConfig& cfg, size_t reception_buf_size);

    TcpClientClass() = delete;

    virtual~ TcpClientClass();

    int perform_operation() override;

private:

#ifdef _WIN32
    static constexpr int SHUT_SEND = SD_SEND;
    static constexpr int SHUT_RECV = SD_RECEIVE;
    static constexpr int SHUT_BOTH = SD_BOTH;
#else

#endif

    socket_t connect_socket;

    int setup(struct addrinfo& hints) override;


    int perform_send_operation();
    int perform_recv_operation();

    int perform_simple_send_op();
    int perform_echo_recv_operation();

    int common_connection_attempt();
    int common_tcp_client_setup(struct addrinfo& hints, socket_t& conn_sock);
};
