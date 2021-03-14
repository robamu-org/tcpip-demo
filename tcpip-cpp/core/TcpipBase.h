#pragma once

#include <demo_config.h>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(__unix__)
#include <netdb.h>
#endif

/**
 * Generic base class for TCPIP classes like. Encapsulates common components to reduce
 * boilerplate and duplicate code.
 */
class TcpipBase {
public:
    TcpipBase() = delete;

    TcpipBase(tcpip::DemoConfig& cfg, size_t reception_buf_size);

    virtual~ TcpipBase();

    virtual int perform_operation() = 0;


protected:

    virtual int setup(struct addrinfo& hints) = 0;

    tcpip::DemoProtocols prot;
    tcpip::DemoTypes type;
    tcpip::DemoModes mode;
    std::string server_address;
    std::string server_port;

    std::vector<uint8_t> reception_buffer;
};
