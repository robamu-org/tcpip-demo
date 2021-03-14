#include "TcpClientClass.h"

#ifdef __unix__
#include <arpa/inet.h>
#endif

#include <iostream>

TcpClientClass::TcpClientClass(tcpip::DemoConfig& cfg, size_t reception_buf_size):
        TcpipBase(cfg, reception_buf_size) {
}

int TcpClientClass::common_connection_attempt() {
    struct addrinfo hints = {};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    return setup(hints);
}

int TcpClientClass::setup(struct addrinfo& hints) {
    return common_tcp_client_setup(hints, connect_socket);
}


int TcpClientClass::common_tcp_client_setup(struct addrinfo& hints, socket_t& conn_sock) {
    struct addrinfo* result = nullptr;
    int retval = 0;

    if(server_address == "" or server_address == "nullptr") {
        int retval = getaddrinfo(nullptr, server_port.c_str(), &hints, &result);
    }
    else {
        int retval = getaddrinfo(server_address.c_str(), server_port.c_str(), &hints, &result);
    }
    if(retval != 0) {
        std::cerr << "getaddrinfo failed with error: " << retval << std::endl;
    }

    for(auto ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *)ptr->ai_addr;
        char *ip = inet_ntoa(addr_in->sin_addr);
        std::cout << "Client: Attempting connection to address " << ip << std::endl;
        /* Create a socket for connecting to server */
        conn_sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (conn_sock < 0) {
#ifdef _WIN32
            int error =  WSAGetLastError();
#elif defined(__unix__)
            int error = errno;
#endif
            std::cerr << ANSI_COLOR_YELLOW << "TcpipBase::common_tcp_client_setup: "
                    "socketfailed with error: " << error << std::endl;
            return 1;
        }

        /* Connect to server. */
        retval = connect(conn_sock, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (retval != 0) {
            conn_sock = INVALID_SOCKET;
            continue;
        }

        std::cout << "Client: Connected successfully to " << ip << std::endl;
        break;
    }

    freeaddrinfo(result);

    if (conn_sock < 0) {
        std::cout << "Unable to connect to server!" << std::endl;
        return 1;
    }
    return 0;
}
