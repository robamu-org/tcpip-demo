#include "utility.h"
#include "demoConfig.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <iostream>
#include <thread>

/* Forward declarations */
int startupCode(tcpip::DemoConfig& cfg);
#if CLI_MODE == 1
int determine_config_from_user(tcpip::DemoConfig& cfg);
#endif
int serverApplication(tcpip::DemoConfig cfg);
int clientApplication(tcpip::DemoConfig cfg);
void printModeInfo(tcpip::DemoConfig& cfg);
#ifdef _WIN32
void enableWinTermColors();
#endif

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
  int result = startupCode(cfg);
  if(result != 0) {
    return result;
  }
#if TCPIP_DEMO_PRINT_MODE_INFO == 1
  printModeInfo(cfg);
#endif

  std::thread server_thread(serverApplication, cfg);
  std::this_thread::sleep_for(1ms);
  std::thread client_thread(clientApplication, cfg);
  server_thread.join();
  client_thread.join();
  spdlog::info("Demo finished");
#if TCPIP_DEMO_AUTOTERMINATE == 0
  std::cout << "Press enter to finish program" << std::endl;
  std::cin.ignore();
#endif
  return 0;
}

int startupCode(tcpip::DemoConfig& cfg) {
  using namespace std;

#ifdef _WIN32
  enableWinTermColors();
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


void printModeInfo(tcpip::DemoConfig& cfg) {
  using md = tcpip::DemoModes;

  if(cfg.prot == tcpip::DemoProtocols::UDP) {
    spdlog::info("Chosen protocol: UDP");
  }
  else if(cfg.prot == tcpip::DemoProtocols::TCP) {
    spdlog::info("Chosen protocol: TCP");
  }

  spdlog::info("Mode information. Selected mode: {}", cfg.mode);

  switch(cfg.mode) {
  case(md::MD_0_PROCEDURAL_DEMO): {
    spdlog::info("Simple procedural demo");
    break;
  }
  case(md::MD_1_OOP_CLIENT_ONE_SERVER_ECHO): {
    spdlog::info("{} will send one packet, {} will echo back", tcpip::CLIENT_PR, tcpip::SERVER_PR);
    break;
  }
  case(md::MD_2_OOP_CLIENT_NONE_SERVER_ONE): {
    spdlog::info("{} will send nothing, {} will send unrequested reply",
        tcpip::CLIENT_PR, tcpip::SERVER_PR);
    break;
  }
  case(md::MD_3_OOP_CLIENT_MUTLIPLE_SERVER_NO_REPLY): {
    spdlog::info("{} will send multiple packets, {} will not reply anything",
        tcpip::CLIENT_PR, tcpip::SERVER_PR);
    break;
  }
  case(md::MD_4_OOP_CLIENT_MUTLIPLE_SERVER_MULTIPLE): {
    spdlog::info("Unimplemented");
    break;
  }
  case(md::MD_5_USE_SELECT): {
    spdlog::info("Echoserver using select");
  }
  }
}
