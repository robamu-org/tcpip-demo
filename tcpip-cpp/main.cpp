#include "utility.h"
#include "demo_config.h"

#include <iostream>
#include <thread>     

/* Forward declarations */
int startup_code(tcpip::DemoConfig& cfg);
int server_application(tcpip::DemoConfig cfg);
int client_application(tcpip::DemoConfig cfg);


int main() {
	using namespace std::chrono_literals;
	using namespace tcpip;
	std::cout << ANSI_COLOR_RESET << "Starting TCP/IP Server/Client C++ demo on " <<
	        get_os_name() << ".." << std::endl;

    DemoConfig cfg;
	startup_code(cfg);

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

int startup_code(tcpip::DemoConfig& cfg) {
    using namespace std;

#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif

#if CLI_MODE == 1
    cout << "Select demo type [h for available types or index]: " << std::endl;
    string mode_string;
    while(true) {
        getline(cin, mode_string);
        if(mode_string == "h") {
            cout << "Available modes: \n";
            cout << "0: Server and Client mode\n";
            cout << "1: Server mode\n";
            cout << "2: Client mode\n";
            cout << endl;
        }
        int mode_index = 0;
        try {
             mode_index = std::stoi(mode_string);
        }
        catch(const std::invalid_argument&) {
            cout << "Invalid argument. Try again." << endl;
            continue;
        }
        if(mode_index < 0 or mode_index > 2) {
            cout << "Invalid argument. Try again." << endl;
            continue;
        }
        cfg.mode = static_cast<tcpip::DemoModes>(mode_index);
        break;
    }

    std::cout << "Please type in port (nothing for default port 20715)" << std::endl;

#else

    cfg.type = static_cast<tcpip::DemoType>(TCPIP_DEMO_TYPE);
    cfg.mode = static_cast<tcpip::DemoModes>(TCPIP_DEMO_MODE);
    cfg.serverAddress = tcpip::SERVER_ADDRESS;
    cfg.serverPort = tcpip::SERVER_PORT;
#endif
    return 0;
}
