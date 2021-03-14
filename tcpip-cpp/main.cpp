#include "utility.h"
#include "demo_config.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <thread>

/* Forward declarations */
int startup_code(tcpip::DemoConfig& cfg);
int determine_config_from_user(tcpip::DemoConfig& cfg);
int server_application(tcpip::DemoConfig cfg);
int client_application(tcpip::DemoConfig cfg);


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
    determine_config_from_user(cfg);
#else
    cfg.type = static_cast<tcpip::DemoTypes>(TCPIP_DEMO_TYPE);
    cfg.mode = static_cast<tcpip::DemoModes>(TCPIP_DEMO_MODE);
    cfg.server_address = tcpip::SERVER_ADDRESS;
    cfg.server_port = tcpip::SERVER_PORT;
#endif
    return 0;
}

int determine_config_from_user(tcpip::DemoConfig& cfg) {
    using namespace std;
    string type_string;
    while(true) {
        cout << "Select demo type [h for available types or index]: " << std::endl;
        getline(cin, type_string);
        if(type_string == "h") {
            cout << "Available types: \n";
            cout << "0: Server and Client type\n";
            cout << "1: Server type\n";
            cout << "2: Client type\n";
            cout << endl;
            continue;
        }
        int type_index = 0;
        try {
             type_index = std::stoi(type_string);
        }
        catch(const std::invalid_argument&) {
            cout << "Invalid argument. Try again." << endl;
            continue;
        }
        if(type_index < 0 or type_index > 2) {
            cout << "Invalid argument. Try again." << endl;
            continue;
        }
        cfg.type = static_cast<tcpip::DemoTypes>(type_index);
        break;
    }

    string mode_string;
    while(true) {
        cout << "Please enter demo mode [h for modes or index]: " << endl;
        getline(cin, mode_string);
        if(mode_string == "h") {
            cout << "Available modes: \n";
            cout << "0: Simple oneshot echo\n";
            cout << "1: OOP Client One Server Reply\n";
            cout << "2: OOP Client None Server Reply \n";
            cout << "3: OOP Client Multiple Server None\n";
            cout << "4: OOP client Multiple Server Multiple\n";
            cout << endl;
            continue;
        }
        int mode_index = 0;
        try {
             mode_index = std::stoi(mode_string);
        }
        catch(const std::invalid_argument&) {
            cout << "Invalid argument. Try again." << endl;
            continue;
        }
        if(mode_index < 0 or mode_index > 4) {
            cout << "Invalid argument. Try again." << endl;
            continue;
        }
        cfg.mode = static_cast<tcpip::DemoModes>(mode_index);
        break;
    }


    while(true) {
        cout << "Please type in port [nothing for default port from demo_config.h]:" << endl;
        string port_string;
        getline(cin, port_string);
        if(port_string.empty()) {
            port_string = tcpip::SERVER_PORT;
        }
        cout << "Confirm port [y/Y]: " << port_string << endl;
        string confirmation;
        getline(cin, confirmation);
        if(confirmation == "y" or confirmation == "Y" or confirmation == "1") {
            break;
        }
    }

    while(true) {
        cout << "Please type in address [nothing for localhost, localhost, any or IP address]" <<
                endl;
        string ip_address;
        getline(cin, ip_address);
        if(ip_address == "") {
            ip_address = "localhost";
        }

        cout << "Confirm address [y/Y]: " << ip_address << endl;
        string confirmation;
        getline(cin, confirmation);
        if(confirmation == "y" or confirmation == "Y" or confirmation == "1") {
            break;
        }
    }
    return 0;
}
