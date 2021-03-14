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

    struct sockaddr sender;
    int sender_sock_len = 0;

    std::mutex packet_lock;
    std::queue<std::vector<uint8_t>> packet_queue;

    int setup(struct addrinfo& hints) override;

    int setup_server();
    int listener_function();
    int packet_handler();

    socket_t server_socket = INVALID_SOCKET;
};



#endif /* CORE_UDP_UDPSERVERCLASS_H_ */
