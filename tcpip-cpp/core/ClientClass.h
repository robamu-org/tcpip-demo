#pragma once

#include <demo_config.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <vector>

class ClientClass {
public:

    ClientClass(tcpip::DemoConfig& cfg);

    virtual~ ClientClass();

    int perform_operation();
private:
    tcpip::DemoModes mode;
    std::string server_address;
    std::string server_port;

#ifdef _WIN32
    SOCKET connect_socket = INVALID_SOCKET;
#else
    int connect_socket = 0;
#endif

    std::vector<uint8_t> reception_buffer;

    int attempt_connection();
    int perform_send_operation(tcpip::DemoModes mode);
    int perform_recv_operation(tcpip::DemoModes mode);

    int perform_simple_send_op();
    int perform_echo_recv_operation();
};
