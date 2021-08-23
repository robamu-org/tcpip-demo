#include "demo_config.h"
#include "spdlog/spdlog.h"

#include <core/tcp/TcpServerClass.h>
#include <core/udp/UdpServerClass.h>

#include <iostream>

int tcpServerOneshot(std::string ip_address);
int udpServerOneshot(std::string ip_address);

int serverApplication(tcpip::DemoConfig cfg) {
    using namespace tcpip;

    spdlog::info("{} application startup..", tcpip::SERVER_PR);

    if(cfg.type == DemoTypes::SERVER_ONLY) {
        return 0;
    }

    if(cfg.mode == DemoModes::MD_0_PROCEDURAL_DEMO) {
        if(cfg.prot == DemoProtocols::TCP) {
            return tcpServerOneshot(cfg.server_address);
        }
        else {
            return udpServerOneshot(cfg.server_address);
        }
    }
    else {
        if(cfg.prot == DemoProtocols::TCP) {
            TcpServerClass tcpServer(cfg, tcpip::BUFFER_SIZES);
            return tcpServer.performOperation();
        }
        else if(cfg.prot == DemoProtocols::UDP) {
            UdpServerClass udpServer(cfg, tcpip::BUFFER_SIZES);
            return udpServer.performOperation();
        }
        else {
            std::cerr << "server_application: Unknown or unimplemented protocol!" << std::endl;
            return 1;
        }
    }
}

