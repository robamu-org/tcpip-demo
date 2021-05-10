#ifndef CORE_UDP_UDPSERVERCLASS_H_
#define CORE_UDP_UDPSERVERCLASS_H_

#include <core/TcpipBase.h>

#include <mutex>
#include <thread>
#include <queue>
#include <vector>

class UdpServerClass: public TcpipBase {
public:
    UdpServerClass(tcpip::DemoConfig& cfg, size_t rec_buf_size);

    UdpServerClass() = delete;

    virtual~ UdpServerClass();

    int perform_operation() override;

private:

    int udp_flags = 0;
    int send_flags = 0;

    socket_t server_socket = INVALID_SOCKET;

    struct sockaddr sender;
    socklen_t sender_sock_len = sizeof(sender);
    size_t bytes_to_send = 0;

    int setup(struct addrinfo& hints) override;

    int setup_server();

    int listen_for_packets();
    int perform_echo_operation();
};



#endif /* CORE_UDP_UDPSERVERCLASS_H_ */
