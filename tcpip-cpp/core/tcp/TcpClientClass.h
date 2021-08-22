#pragma once

#include <core/TcpipBase.h>
#include <demo_config.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <queue>
#include <vector>

class TcpClientClass: public TcpipBase {
public:

    TcpClientClass(tcpip::DemoConfig& cfg, size_t reception_buf_size);

    TcpClientClass() = delete;

    virtual~ TcpClientClass();

    int performOperation() override;

private:

    enum Steps {
        SEND,
        READ
    };

    socket_t connectSocket = INVALID_SOCKET;

    int setup(struct addrinfo& hints) override;

    int performSendOperation();
    int performRecvOperation();
    int performOpCommon(Steps step);

    int performSimpleSendOp();
    int sendPackets(std::queue<std::string>& packet_queue);
    int tcpReadUntilServerFinishedOperation();

    int commonConnectionAttempt();

};
