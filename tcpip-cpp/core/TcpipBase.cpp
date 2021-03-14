#include "TcpipBase.h"

#ifdef __unix__
#include <arpa/inet.h>
#endif

#include <iostream>

TcpipBase::TcpipBase(tcpip::DemoConfig &cfg, size_t reception_buf_size):
        prot(cfg.prot), type(cfg.type), mode(cfg.mode),
        server_address(cfg.server_address), server_port(cfg.server_port),
        reception_buffer(reception_buf_size) {
#ifdef _WIN32
    WSADATA wsaData;
    /* Initialize Winsock */
    int retval = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (retval != 0) {
        std::cerr << "WSAStartup failed with error: " << retval << std::endl;
        /* Should not happen, terminate program */
        throw std::runtime_error("WSAStartup failed!");
    }
#endif
}

TcpipBase::~TcpipBase() {
#ifdef _WIN32
    WSACleanup();
#endif
}

int TcpipBase::common_tcp_client_setup(struct addrinfo& hints, int& conn_sock) {
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
        if (conn_sock == INVALID_SOCKET) {
#ifdef _WIN32
            int error =  WSAGetLastError();
#elif defined(__unix__)
            int error = errno;
#endif
            std::cerr << "socket failed with error: "<< error << std::endl;
            return 1;
        }

        /* Connect to server. */
        retval = connect(conn_sock, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (retval == SOCKET_ERROR) {
            conn_sock = INVALID_SOCKET;
            continue;
        }

        std::cout << "Client: Connected successfully to " << ip << std::endl;
        break;
    }

    freeaddrinfo(result);

    if (conn_sock == INVALID_SOCKET) {
        std::cout << "Unable to connect to server!" << std::endl;
        return 1;
    }
    return 0;
}
