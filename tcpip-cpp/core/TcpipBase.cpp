#include "TcpipBase.h"

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

