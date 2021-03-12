#include "utility.h"

#include <iostream>
#include <thread>     

/* Forward declarations */
int server_application();
int client_application();

int main() {
	std::cout << "Starting TCP/IP Server/Client demo on " << get_os_name() << ".." << std::endl;
	std::thread server_thread(server_application);
	std::thread client_thread(client_application);
	server_thread.join();
	client_thread.join();
	std::cout << "Demo finished!" << std::endl;
	return 0;
}
