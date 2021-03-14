#include "demo_config.h"

#include <core/tcp/TcpServerClass.h>
#include <core/udp/UdpServerClass.h>

#include <iostream>

int server_oneshot(std::string ip_address);

int server_application(tcpip::DemoConfig cfg) {
    using namespace tcpip;
    std::cout << ANSI_COLOR_GREEN << "Server application startup.." << std::endl;

    if(cfg.type == DemoTypes::SERVER_ONLY) {
        return 0;
    }

    if(cfg.mode == DemoModes::MD_0_PROCEDURAL_DEMO) {
        return server_oneshot(cfg.server_address);
    }
    else {
        if(cfg.prot == DemoProtocols::TCP) {
            TcpServerClass tcpServer(cfg, tcpip::BUFFER_SIZES);
            return tcpServer.perform_operation();
        }
        else if(cfg.prot == DemoProtocols::UDP) {
            UdpServerClass udpServer(cfg, tcpip::BUFFER_SIZES);
            return udpServer.perform_operation();
        }
        else {
            std::cerr << "server_application: Unknown or unimplemented protocol!" << std::endl;
            return 1;
        }
    }
}

