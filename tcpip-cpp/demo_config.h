#pragma once

#include "definitions.h"
#include <cstddef>

namespace tcpip {

static constexpr size_t BUFFER_SIZES = 1500;
constexpr char SERVER_ADDRESS[] = "localhost";
constexpr char SERVER_PORT[] = "27015";

}

//! In CLI mode, the CL interface is used to prompt data from the user instead of hardcoding
//! the configuration in this header file.
#define CLI_MODE                                    0

//! These values will be prompted from the user in CLI mode.
#define TCPIP_DEMO_PROT                             TCPIP_PROT_1_UDP
#define TCPIP_DEMO_TYPE                             TCPIP_TP_0_CLIENT_AND_SERVER
#define TCPIP_DEMO_MODE                             TCPIP_MD_1_OOP_CLIENT_ONE_SERVER_ECHO

//! If this in 1, the program will exit automatically instead of prompting an enter command
#define TCPIP_DEMO_AUTOTERMINATE                    1

//! Print what the mode does
#define TCPIP_DEMO_PRINT_MODE_INFO                  1




