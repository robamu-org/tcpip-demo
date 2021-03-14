#include "../TcpClientClass.h"
#include <utility.h>
#include <unistd.h>

#include <iostream>
#include <iomanip>

TcpClientClass::TcpClientClass(tcpip::DemoConfig& cfg, size_t reception_buffer_size):
        TcpipBase(cfg, reception_buffer_size) {
}

TcpClientClass::~TcpClientClass() {
    close(connect_socket);

}

int TcpClientClass::perform_operation() {
    if(type == tcpip::DemoTypes::SERVER_ONLY) {
        return 0;
    }

    int retval = attempt_connection();
    if(retval != 0) {
        return retval;
    }

    std::cout << CL_CLR << "Client: " << std::put_time(get_local_time(), "%H:%M:%S") << " | "
            << "Connection to server established" << std::endl;
    return 0;
}

int TcpClientClass::setup(struct addrinfo &hints) {
    return 0;
}

int TcpClientClass::attempt_connection() {
    return 0;
}

int TcpClientClass::perform_send_operation() {
    return 0;
}

int TcpClientClass::perform_recv_operation() {
    return 0;
}

int TcpClientClass::perform_simple_send_op() {
    return 0;
}

int TcpClientClass::perform_echo_recv_operation() {
    return 0;
}
