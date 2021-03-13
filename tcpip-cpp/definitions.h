#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include <string>

namespace tcpip {

enum DemoType {
    CLIENT_ONLY,
    SERVER_ONLY,
    CLIENT_AND_SERVER
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
#define TCPIP_CLIENT_ONLY                           0
#define TCPIP_SERVER_ONLY                           1
#define TCPIP_CLIENT_AND_SERVER                     2

//! Demo modes
//! Simple, procedural
#define TCPIP_PROCEDURAL_DEMO                       0
//! Same demo as above but implemented using classes and modern C++
#define TCPIP_OOP_CLIENT_ONE_SERVER_ECHO            1
#define TCPIP_OOP_CLIENT_NONE_SERVER_ONE            2
#define TCPIP_OOP_CLIENT_MUTLIPLE_SERVER_NO_REPLY   3
#define TCPIP_OOP_CLIENT_MUTLIPLE_SERVER_MULTIPLE   4

#endif /* DEFINITIONS_H_ */
