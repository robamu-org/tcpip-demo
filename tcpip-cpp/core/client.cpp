#include <core/tcp/TcpClientClass.h>
#include "demo_config.h"
#include <iostream>

int client_oneshot(std::string server_address);

int client_application(tcpip::DemoConfig cfg) {
    using namespace tcpip;
	std::cout << ANSI_COLOR_CYAN << "Client application startup.." << std::endl;
    
	if(cfg.type == DemoTypes::SERVER_ONLY) {
	    return 0;
	}

    if(cfg.mode == DemoModes::MD_0_PROCEDURAL_DEMO) {
        return client_oneshot(cfg.server_address);
    }
    else {
        if(cfg.prot == DemoProtocols::TCP) {
            TcpClientClass tcpClient(cfg, tcpip::BUFFER_SIZES);
            return tcpClient.perform_operation();
        }
        else {
            std::cerr << "server_application: Protocol not implemented yet!" << std::endl;
            return 0;
        }
    }
}

