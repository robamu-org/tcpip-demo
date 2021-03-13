#include "demo_config.h"
#include <iostream>

int client_oneshot();

int client_application(tcpip::DemoConfig cfg) {
    using namespace tcpip;
	std::cout << ANSI_COLOR_CYAN << "Client application startup.." << std::endl;
    
	if(cfg.type == DemoType::SERVER_ONLY) {
	    return 0;
	}

	switch(cfg.mode) {
	case(DemoModes::PROCEDURAL_DEMO): {
	    return client_oneshot();
	}
	case(DemoModes::OOP_CLIENT_ONE_SERVER_ECHO): {
	    return client_oneshot();
	}
	case(DemoModes::OOP_CLIENT_NONE_SERVER_ONE):
	case(DemoModes::OOP_CLIENT_MUTLIPLE_SERVER_NO_REPLY):
	case(DemoModes::OOP_CLIENT_MUTLIPLE_SERVER_MULTIPLE):
	default: {
	    std::cerr << "client_application: Mode not implemented yet!" << std::endl;
	    return -1;
	}
	}
}

