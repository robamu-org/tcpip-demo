#pragma once

#include <cstddef>

namespace tcpip {

enum Modes {
    //! Simple demo, but executed by class
    CLIENT_ONE_PACKET_SERVER_ECHO,
    CLIENT_NO_DATA_SERVER_REPLY,
    CLIENT_MUTLIPLE_DATA_SERVER_NO_REPLY,
    CLIENT_MULTIPLE_DATA_SERVER_MULTIPLE_REPLY
};

static constexpr size_t BUFFER_SIZES = 1500;
constexpr char SERVER_ADDRESS[] = "localhost";
constexpr char SERVER_PORT[] = "27015";

}

//! In CLI mode, the CL interface is used to prompt data from the user instead of hardcoding
//! the configuration in this header file. Not implemented yet.
#define CLI_MODE                                    0

//! Simple, procedural
#define TCPIP_SIMPLE_ONE_SHOT_DEMO                  0
//! Same demo as above but implemented using classes and modern C++
#define TCPIP_CLASS_ONE_SHOT_DEMO                   1
#define TCPIP_SERVER_REPLYING_DEMO                  2

#define TCPIP_DEMO_TYPE                             TCPIP_SIMPLE_ONE_SHOT_DEMO

/* If this in 1, the program will exit automatically instead of prompting an enter command */
#define AUTO_TERMINATE_DEMO                         1




