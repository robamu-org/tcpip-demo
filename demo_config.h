#include <cstddef>

//! Simple stupid, procedural and usable from C
#define TCPIP_SIMPLE_ONE_SHOT_DEMO                    0
//! Same demo as above but implemented using classes and modern C++
#define TCPIP_CLASS_ONE_SHOT_DEMO                     1
#define TCPIP_SERVER_REPLYING_DEMO                    2

#define TCPIP_DEMO_TYPE                               TCPIP_CLASS_ONE_SHOT_DEMO

static constexpr size_t TCPIP_BUFFER_SIZES = 1500;

constexpr char TCPIP_SERVER_ADDRESS[] = "localhost";
constexpr char TCPIP_SERVER_PORT[] = "27015";
