#include <cstddef>

#define SIMPLE_ONE_SHOT_DEMO                    0
#define SERVER_REPLYING_DEMO                    1

#define DEMO_TYPE                               SIMPLE_ONE_SHOT_DEMO

static constexpr size_t TCPIP_BUFFER_SIZES = 1500;

constexpr char TCPIP_SERVER_ADDRESS[] = "localhost";
constexpr char TCPIP_SERVER_PORT[] = "27015";