#include "utility.h"
#include "demo_config.h"

#include <iostream>
#include <thread>     

/* Forward declarations */
int server_application(tcpip::DemoConfig cfg);
int client_application(tcpip::DemoConfig cfg);

int main() {
	using namespace std::chrono_literals;
	using namespace tcpip;
	std::cout << "Starting TCP/IP Server/Client C++ demo on " << get_os_name() << ".." << std::endl;

#if CLI_MODE == 1
#else
	DemoConfig cfg;
	cfg.type = static_cast<tcpip::DemoType>(TCPIP_DEMO_TYPE);
	cfg.mode = static_cast<tcpip::DemoModes>(TCPIP_DEMO_MODE);
	cfg.serverAddress = tcpip::SERVER_ADDRESS;
	cfg.serverPort = tcpip::SERVER_PORT;
#endif

	std::thread server_thread(server_application, cfg);
	std::this_thread::sleep_for(10ms);
	std::thread client_thread(client_application, cfg);
	server_thread.join();
	client_thread.join();
	std::cout << "Demo finished." << std::endl;
#if AUTO_TERMINATE_DEMO == 0
	std::cout << "Press enter to finish program" << std::endl;
	std::cin.ignore();
#endif
	return 0;
}
