#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include <string>

#define ANSI_COLOR_RED      "\x1b[31m"
#define ANSI_COLOR_GREEN    "\x1b[32m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_BLUE     "\x1b[34m"
#define ANSI_COLOR_MAGENTA  "\x1b[35m"
#define ANSI_COLOR_CYAN     "\x1b[36m"
#define ANSI_COLOR_RESET    "\x1b[0m"

namespace tcpip {

enum DemoType {
    CLIENT_AND_SERVER,
    SERVER_ONLY,
    CLIENT_ONLY,
};

enum DemoModes {
    //! Simplest demo, procedural one shot with echo handling
    PROCEDURAL_DEMO,
    //! Now more complex demos implemented with classes
    OOP_CLIENT_ONE_SERVER_ECHO,
    OOP_CLIENT_NONE_SERVER_ONE,
    OOP_CLIENT_MUTLIPLE_SERVER_NO_REPLY,
    OOP_CLIENT_MUTLIPLE_SERVER_MULTIPLE
};

struct DemoConfig {
    tcpip::DemoType type;
    tcpip::DemoModes mode;
    std::string serverAddress;
    std::string serverPort;
};

}

//! Demo types
#define TCPIP_TP_0_CLIENT_AND_SERVER                     0
#define TCPIP_TP_1_SERVER_ONLY                           1
#define TCPIP_TP_2_CLIENT_ONLY                           2


//! Demo modes
//! Simple, procedural
#define TCPIP_MD_0_PROCEDURAL_DEMO                       0
//! Same demo as above but implemented using classes and modern C++
#define TCPIP_MD_1_OOP_CLIENT_ONE_SERVER_ECHO            1
#define TCPIP_MD_2_OOP_CLIENT_NONE_SERVER_ONE            2
#define TCPIP_MD_3_OOP_CLIENT_MUTLIPLE_SERVER_NO_REPLY   3
#define TCPIP_MD_4_OOP_CLIENT_MUTLIPLE_SERVER_MULTIPLE   4

#endif /* DEFINITIONS_H_ */
