#include "UdpClientClass.h"

#include <iostream>

UdpClientClass::UdpClientClass(tcpip::DemoConfig &cfg, size_t rec_buf_size):
        TcpipBase(cfg, rec_buf_size) {
}

UdpClientClass::~UdpClientClass() {
    tcpip::close_socket(server_socket);
    freeaddrinfo(server_addr);
}

int UdpClientClass::perform_operation() {
    if(type == tcpip::DemoTypes::SERVER_ONLY) {
        return 0;
    }

    int retval = setup_client();
    if(retval != 0) {
        return retval;
    }

    retval = perform_send_operation();
    if(retval != 0) {
        return retval;
    }

    return listen_for_replies();
}

int UdpClientClass::setup_client() {
    struct addrinfo hints = {};

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    return setup(hints);
}


int UdpClientClass::setup(struct addrinfo &hints) {
    /* Resolve the server address and port */
    int retval = 0;
    if(server_address == "any" or server_address == "") {
        retval = getaddrinfo(nullptr, server_port.c_str(), &hints, &server_addr);
    }
    else {
        retval = getaddrinfo(server_address.c_str(), server_port.c_str(), &hints, &server_addr);
    }

    if (retval != 0) {
        std::cerr << "TcpServerClass::setup_server: getaddrinfo failed with error: " <<
                retval << std::endl;
        return 1;
    }
    /* Create a socket for connecting to server */
    server_socket = socket(server_addr->ai_family, server_addr->ai_socktype,
            server_addr->ai_protocol);
    if (server_socket < 0) {
        std::cerr << "TcpServerClass::setup_server: socket failed with error: " <<
                tcpip::get_last_error() << std::endl;
        freeaddrinfo(server_addr);
        return 1;
    }
    return 0;
}

int UdpClientClass::perform_send_operation() {
    std::string string = "This is a UDP test";
    std::cout << "Client: Sending string with " << string.size() << " bytes: " <<
            string << std::endl;
    int retval = sendto(server_socket, string.data(), string.size(), 0, server_addr->ai_addr,
            server_addr->ai_addrlen);
    if(retval > 0) {
        std::cout << "Client: Sent string with " << string.size() << " bytes successfully"
                << std::endl;
    }
    else {
        std::cerr << "UdpClientClass::perform_send_operation: sendto failed with error: " <<
                tcpip::get_last_error() << std::endl;
    }
    return 0;
}

int UdpClientClass::listen_for_replies() {
    int recv_flags = 0;
    int retval = recv(
            server_socket,
            reinterpret_cast<char*>(reception_buffer.data()),
            reception_buffer.capacity() - 1,
            recv_flags
    );
    if(retval < 0) {
        std::cerr << "UdpClientClass::listen_for_replies: recv failed with error: " <<
                tcpip::get_last_error() << std::endl;
    }
    reception_buffer[retval] = '\0';
    std::cout << CL_CLR << "Client: Received back " << retval << " bytes: " <<
            reception_buffer.data() << std::endl;
    return 0;
}
