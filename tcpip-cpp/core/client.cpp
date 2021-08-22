#include "demo_config.h"
#include <iostream>

#include <core/tcp/TcpClientClass.h>
#include <core/udp/UdpClientClass.h>

int tcpClientOneshot(std::string server_address);
int udpClientOneshot(std::string server_address);

int clientApplication(tcpip::DemoConfig cfg) {
    using namespace tcpip;
	std::cout << ANSI_COLOR_CYAN << "Client application startup.." << std::endl;
    
	if(cfg.type == DemoTypes::SERVER_ONLY) {
	    return 0;
	}

    if(cfg.mode == DemoModes::MD_0_PROCEDURAL_DEMO) {
        if(cfg.prot == DemoProtocols::TCP) {
            return tcpClientOneshot(cfg.server_address);
        }
        else {
            return udpClientOneshot(cfg.server_address);
        }
    }
    else {
        if(cfg.prot == DemoProtocols::TCP) {
            TcpClientClass tcpClient(cfg, tcpip::BUFFER_SIZES);
            return tcpClient.performOperation();
        }
        else if(cfg.prot == DemoProtocols::UDP) {
            UdpClientClass udpClient(cfg, tcpip::BUFFER_SIZES);
            return udpClient.performOperation();
        }
        else {
            std::cerr << "client_application: Protocol unknown or not implemented"
                    " yet!" << std::endl;
            return 0;
        }
    }
}

