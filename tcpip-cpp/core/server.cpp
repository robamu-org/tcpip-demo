#include "demo_config.h"
#include <core/ServerClass.h>
#include <iostream>

int server_oneshot();

int server_application() {
    std::cout << "Server application startup.." << std::endl;

#if TCPIP_DEMO_TYPE == TCPIP_SIMPLE_ONE_SHOT_DEMO
    /* Simple one-shot demo, no classes or any fancy stuff */
    return server_oneshot();

#elif TCPIP_DEMO_TYPE == TCPIP_CLASS_ONE_SHOT_DEMO

    ServerClass serverClass(tcpip::Modes::CLIENT_ONE_PACKET_SERVER_ECHO);
    return serverClass.perform_operation();

#endif

}

