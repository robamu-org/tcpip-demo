#include "../ClientClass.h"
#include <iostream>

ClientClass::ClientClass(tcpip::DemoConfig& cfg): mode(cfg.mode),
        server_address(cfg.server_address), server_port(cfg.server_port),
        reception_buffer(tcpip::BUFFER_SIZES) {
    WSADATA wsaData;
    // Initialize Winsock
    int retval = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (retval != 0) {
        std::cerr << "WSAStartup failed with error: " << retval << std::endl;
        throw std::runtime_error("WSAStartup failed!");
    }
}

ClientClass::~ClientClass() {
    closesocket(connect_socket);
    WSACleanup();
}

int ClientClass::performOperation() {
    int retval = 0;

    struct addrinfo *result = nullptr;
    struct addrinfo hints = {};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if(server_address == "" or server_address == "nullptr") {
        int retval = getaddrinfo(nullptr, server_port.c_str(), &hints, &result);
    }
    else {
        int retval = getaddrinfo(server_address.c_str(), server_port.c_str(), &hints, &result);
    }
    if(retval != 0) {
        std::cerr << "getaddrinfo failed with error: " << retval << std::endl;
    }

    return 0;
}
