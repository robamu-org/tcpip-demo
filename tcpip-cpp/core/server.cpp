#include <core/tcp/TcpServerClass.h>
#include "demo_config.h"
#include <iostream>

int server_oneshot(std::string ip_address);

int server_application(tcpip::DemoConfig cfg) {
    using namespace tcpip;
    std::cout << ANSI_COLOR_GREEN << "Server application startup.." << std::endl;

    if(cfg.type == DemoTypes::SERVER_ONLY) {
        return 0;
    }

    switch(cfg.mode) {
    case(DemoModes::MD_0_PROCEDURAL_DEMO): {
        return server_oneshot(cfg.server_address);
    }
    case(DemoModes::MD_1_OOP_CLIENT_ONE_SERVER_ECHO): {
        if(cfg.prot == DemoProtocols::TCP) {
            TcpServerClass serverClass(cfg, tcpip::BUFFER_SIZES);
            return serverClass.perform_operation();
        }
        else {
            std::cerr << "server_application: Protocol not implemented yet!" << std::endl;
            return 0;
        }
    }
    case(DemoModes::MD_2_OOP_CLIENT_NONE_SERVER_ONE):
    case(DemoModes::MD_3_OOP_CLIENT_MUTLIPLE_SERVER_NO_REPLY):
    case(DemoModes::MD_4_OOP_CLIENT_MUTLIPLE_SERVER_MULTIPLE):
    default: {
        std::cerr << "client_application: Mode not implemented yet!" << std::endl;
        return -1;
    }
    }
}

