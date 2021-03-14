#include <core/tcp/TcpClientClass.h>
#include "demo_config.h"
#include <iostream>

int client_oneshot();

int client_application(tcpip::DemoConfig cfg) {
    using namespace tcpip;
	std::cout << ANSI_COLOR_CYAN << "Client application startup.." << std::endl;
    
	if(cfg.type == DemoTypes::SERVER_ONLY) {
	    return 0;
	}

	switch(cfg.mode) {
	case(DemoModes::MD_0_PROCEDURAL_DEMO): {
	    return client_oneshot();
	}
	case(DemoModes::MD_1_OOP_CLIENT_ONE_SERVER_ECHO): {
	    if(cfg.prot == DemoProtocols::TCP){
	        //TcpClientClass clientClass(cfg, tcpip::BUFFER_SIZES);
	        //return clientClass.perform_operation();
	        return client_oneshot();
	    }
	    else {
	        std::cerr << "client_application: Protocol not implemented yet!" << std::endl;
	    }
	    return 0;
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

