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
#define CLI_MODE                                    1

//! These values will be prompted from the user in CLI mode.
#define TCPIP_DEMO_MODE                             TCPIP_MD_0_PROCEDURAL_DEMO
#define TCPIP_DEMO_TYPE                             TCPIP_TP_0_CLIENT_AND_SERVER

//! If this in 1, the program will exit automatically instead of prompting an enter command
#define AUTO_TERMINATE_DEMO                         1




