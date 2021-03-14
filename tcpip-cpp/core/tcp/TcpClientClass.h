#pragma once

#include <core/TcpipBase.h>
#include <demo_config.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <queue>
#include <vector>

class TcpClientClass: public TcpipBase {
public:

    TcpClientClass(tcpip::DemoConfig& cfg, size_t reception_buf_size);

    TcpClientClass() = delete;

    virtual~ TcpClientClass();

    int perform_operation() override;

private:

    enum Steps {
        SEND,
        READ
    };

    socket_t connect_socket = INVALID_SOCKET;

    int setup(struct addrinfo& hints) override;

    int perform_send_operation();
    int perform_recv_operation();
    int perform_op_common(Steps step);

    int perform_simple_send_op();
    int send_packets(std::queue<std::string>& packet_queue);
    int tcp_read_operation();

    int common_connection_attempt();
    int common_tcp_client_setup(struct addrinfo& hints, socket_t& conn_sock);

};
