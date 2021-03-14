#pragma once

#include <demo_config.h>
#include "common.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <vector>

class ClientClass {
public:

    ClientClass(tcpip::DemoConfig& cfg);

    virtual~ ClientClass();

    int performOperation();
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

};
