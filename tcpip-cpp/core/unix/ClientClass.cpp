#include "../ClientClass.h"

#include <iostream>

ClientClass::ClientClass(tcpip::DemoModes mode): mode(mode),
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

}

int ClientClass::performOperation() {
    return 0;
}
