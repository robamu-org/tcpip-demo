#include "../TcpClientClass.h"

#include <unistd.h>

#include <iostream>

TcpClientClass::TcpClientClass(tcpip::DemoConfig& cfg, size_t reception_buffer_size):
        TcpipBase(cfg, reception_buffer_size) {
}

TcpClientClass::~TcpClientClass() {
    close(connect_socket);

}

int TcpClientClass::perform_operation() {
    return 0;
}

int TcpClientClass::setup(struct addrinfo &hints) {
    return 0;
}
