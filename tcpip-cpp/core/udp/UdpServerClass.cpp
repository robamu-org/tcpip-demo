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

    retval = listen_for_packets();
    if(retval != 0) {
        return retval;
    }

    return perform_echo_operation();
}

int UdpServerClass::setup_server() {
    struct addrinfo hints = {};

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

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

int UdpServerClass::listen_for_packets() {
    std::cout << "Server: Listening for UDP packets.." << std::endl;
    int error = 0;
    int retval = recvfrom(
            server_socket,
            reinterpret_cast<char*>(reception_buffer.data()),
            reception_buffer.capacity() - 1,
            udp_flags,
            &sender,
            &sender_sock_len
    );

    if(retval < 0) {
        error = tcpip::get_last_error();
        std::cerr << "Client: UdpServerClass::listen_for_packets: recvfrom failed with " <<
                error << std::endl;
        return -1;
    }
    reception_buffer[retval] = '\0';
    std::cout << SRV_CLR <<"Server: Received " << retval << " bytes: " <<
            reception_buffer.data() << std::endl;
    bytes_to_send = retval;
    return 0;
}

int UdpServerClass::perform_echo_operation() {
    int send_flags = 0;
    int send_ret = sendto(
            server_socket,
            reinterpret_cast<char*>(reception_buffer.data()),
            bytes_to_send,
            send_flags,
            &sender,
            sender_sock_len
    );
    if(send_ret < 0) {
        int error = tcpip::get_last_error();
        std::cerr << "Client: UdpServerClass::listen_for_packets: sendto failed with " <<
                error << std::endl;
        return -1;
    }

    std::cout << SRV_CLR << "Server: Echoed back packet" << std::endl;
    return 0;
}
