#include "utility.h"
#include "demo_config.h"

#include <iostream>
#include <thread>     

/* Forward declarations */
int server_application();
int client_application();

int main() {
	using namespace std::chrono_literals;
	std::cout << "Starting TCP/IP Server/Client demo on " << get_os_name() << ".." << std::endl;
	std::thread server_thread(server_application);
	std::this_thread::sleep_for(10ms);
	std::thread client_thread(client_application);
	server_thread.join();
	client_thread.join();
#if AUTO_TERMINATE_DEMO == 0
	std::cout << "Demo finished!\nPress enter to finish program" << std::endl;
	std::cin.ignore();
#endif
	return 0;
}
