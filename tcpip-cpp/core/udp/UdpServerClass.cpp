#include "UdpServerClass.h"

#include <iostream>

UdpServerClass::UdpServerClass(tcpip::DemoConfig &cfg, size_t rec_buf_size):
        TcpipBase(cfg,  rec_buf_size) {
}

UdpServerClass::~UdpServerClass() {
    tcpip::close_socket(server_socket);
}

int UdpServerClass::perform_operation() {
    if(type == tcpip::DemoTypes::CLIENT_ONLY) {
        return 0;
    }

    int retval = setup_server();
    if(retval != 0) {
        return retval;
    }

    std::thread listener_thread(&UdpServerClass::listener_function, this);

    retval = packet_handler();
    if(retval != 0) {
        return retval;
    }

    listener_thread.join();
    return 0;
}

int UdpServerClass::setup_server() {
    struct addrinfo hints = {};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
#ifdef __unix__
    hints.ai_flags = AI_PASSIVE;
#endif
    return setup(hints);
}

int UdpServerClass::setup(struct addrinfo &hints) {
    struct addrinfo *result = nullptr;

    /* Resolve the server address and port */
    int retval = 0;
    if(server_address == "any" or server_address == "") {
        retval = getaddrinfo(nullptr, server_port.c_str(), &hints, &result);
    }
    else {
        retval = getaddrinfo(server_address.c_str(), server_port.c_str(), &hints, &result);
    }

    if (retval != 0) {
        std::cerr << "TcpServerClass::setup_server: getaddrinfo failed with error: " <<
                retval << std::endl;
        return 1;
    }
    /* Create a socket for connecting to server */
    server_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (server_socket < 0) {
        std::cerr << "TcpServerClass::setup_server: socket failed with error: " <<
                errno << std::endl;
        freeaddrinfo(result);
        return 1;
    }

    /* Setup the UDP server socket */
    retval = bind(server_socket, result->ai_addr, (int)result->ai_addrlen);
    if (retval != 0) {
        std::cerr << "TcpServerClass::setup_server: bind failed with error: " <<
                tcpip::get_last_error() << std::endl;
        freeaddrinfo(result);
        return 1;
    }

    freeaddrinfo(result);
    return 0;
}

int UdpServerClass::listener_function() {

    int retval = recvfrom(
            server_socket,
            reinterpret_cast<char*>(reception_buffer.data()),
            reception_buffer.capacity() - 1,
            udp_flags,
            &sender,
            &sender_sock_len
    );

    if(retval > 0) {
        std::lock_guard<std::mutex> lock(packet_lock);
        std::vector<uint8_t> packet(reception_buffer.begin(), reception_buffer.begin() + retval);
        packet_queue.push(packet);
    }
    return 0;
}

int UdpServerClass::packet_handler() {
    std::vector<uint8_t> packet;
    while(true) {
        std::lock_guard<std::mutex> lock(packet_lock);
        if(packet_queue.size() > 0) {
            packet = packet_queue.front();
            int send_ret = sendto(
                    server_socket,
                    reinterpret_cast<const char*>(packet.data()),
                    packet.size(),
                    send_flags,
                    &sender,
                    sender_sock_len
            );
            if(send_ret > 0) {
                packet_queue.pop();
                break;
            }
            else {
                std::cerr << "Server: Sending back packet failed!" << std::endl;
                return -1;
            }
        }
        else {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(50ms);
        }
    }

    std::cout << CL_CLR << "Client: Received and echoed back " << packet.size() << " bytes"<<
            std::endl;
    return 0;
}
