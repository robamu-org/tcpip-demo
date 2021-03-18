#include "utility.h"
#include "demo_config.h"

#include <iostream>
#include <thread>

/* Forward declarations */
int startup_code(tcpip::DemoConfig& cfg);
#if CLI_MODE == 1
int determine_config_from_user(tcpip::DemoConfig& cfg);
#endif
int server_application(tcpip::DemoConfig cfg);
int client_application(tcpip::DemoConfig cfg);
void print_mode_info(tcpip::DemoConfig& cfg);
void enable_win_term_colors();


/**
 * @brief   TCP/IP C++ demo entry point.
 * @details
 * This program can be configured by tweaking the demo_config.h file.
 * @author  R. Mueller
 * @return
 */
int main() {
	using namespace std::chrono_literals;
	using namespace tcpip;
	std::cout << ANSI_COLOR_RESET << "Starting TCP/IP Server/Client C++ demo on " <<
	        get_os_name() << ".." << std::endl;

    DemoConfig cfg;
	int result = startup_code(cfg);
	if(result != 0) {
	    return result;
	}
#if TCPIP_DEMO_PRINT_MODE_INFO == 1
	print_mode_info(cfg);
#endif

	std::thread server_thread(server_application, cfg);
	std::this_thread::sleep_for(1ms);
	std::thread client_thread(client_application, cfg);
	server_thread.join();
	client_thread.join();
	std::cout << ANSI_COLOR_RESET << "Demo finished." << std::endl;
#if TCPIP_DEMO_AUTOTERMINATE == 0
	std::cout << "Press enter to finish program" << std::endl;
	std::cin.ignore();
#endif
	return 0;
}

int startup_code(tcpip::DemoConfig& cfg) {
    using namespace std;

#ifdef _WIN32
    enable_win_term_colors();
#endif

#if CLI_MODE == 1
    determine_config_from_user(cfg);
#else
    cfg.type = static_cast<tcpip::DemoTypes>(TCPIP_DEMO_TYPE);
    cfg.mode = static_cast<tcpip::DemoModes>(TCPIP_DEMO_MODE);
    cfg.prot = static_cast<tcpip::DemoProtocols>(TCPIP_DEMO_PROT);
    cfg.server_address = tcpip::SERVER_ADDRESS;
    cfg.server_port = tcpip::SERVER_PORT;
#endif
    return 0;
}


void print_mode_info(tcpip::DemoConfig& cfg) {
    using md = tcpip::DemoModes;

    if(cfg.prot == tcpip::DemoProtocols::UDP) {
        std::cout << "Chosen protocol: UDP" << std::endl;
    }
    else if(cfg.prot == tcpip::DemoProtocols::TCP) {
        std::cout << "Chosen protocol: TCP" << std::endl;
    }

    std::cout << "Mode information. Selected mode: " << static_cast<int>(cfg.mode) << std::endl;
    std::string server_pr = std::string(SRV_CLR) + "Server" + std::string(ANSI_COLOR_RESET);
    std::string client_pr = std::string(CL_CLR) + "Client" + std::string(ANSI_COLOR_RESET);
    switch(cfg.mode) {
    case(md::MD_1_OOP_CLIENT_ONE_SERVER_ECHO): {
        std::cout << client_pr <<  " will send one packet, " << server_pr << " will echo back." <<
                std::endl;
        break;
    }
    case(md::MD_2_OOP_CLIENT_NONE_SERVER_ONE): {
        std::cout << client_pr << " will send nothing, " << server_pr << " will send "
                "unrequested reply." << std::endl;
        break;
    }
    case(md::MD_3_OOP_CLIENT_MUTLIPLE_SERVER_NO_REPLY): {
        std::cout << client_pr << " will send multiple packets, " << server_pr << " will not reply "
                "anything." << std::endl;
        break;
    }
    case(md::MD_4_OOP_CLIENT_MUTLIPLE_SERVER_MULTIPLE): {
        std::cout << client_pr << " will send multiple packets, " << server_pr << " "
                "echo all of them." << std::endl;
        break;
    }
    }
}
