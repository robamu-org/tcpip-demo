#pragma once

#include <core/TcpipBase.h>
#include <demoConfig.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <vector>
#include <cstdint>

class TcpServerClass: public TcpipBase {
public:

    TcpServerClass(tcpip::DemoConfig& cfg, size_t reception_buf_size);

    TcpServerClass() = delete;

    virtual~ TcpServerClass();

    int performOperation() override;
private:

    socket_t listenSocket = INVALID_SOCKET;
    socket_t clientSocket = INVALID_SOCKET;

    int setupServer();
    int setup(struct addrinfo& hints) override;

    int acceptConnection();
    int performModeOperation();

    int perforMode1Echo();
    int performMode2();
    int performMode3();
    int performMode5();

    int performShutdown();

    int receiveData(uint32_t* index = nullptr);
};
