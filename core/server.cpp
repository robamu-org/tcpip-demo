#include "demo_config.h"
#include <core/win32/ServerClass.h>
#include <iostream>

#if TCPIP_DEMO_TYPE == TCPIP_SIMPLE_ONE_SHOT_DEMO
int server_oneshot_win32();
#endif

int server_application() {
    std::cout << "Server application startup.." << std::endl;

#if TCPIP_DEMO_TYPE == TCPIP_SIMPLE_ONE_SHOT_DEMO
    /* Simple one-shot demo, no classes or any fancy stuff */
#ifdef WIN32
    return server_oneshot_win32();
#elif UNIX
    return 0;
#endif

#elif TCPIP_DEMO_TYPE == TCPIP_CLASS_ONE_SHOT_DEMO

    ServerClass serverClass(tcpip::Modes::CLIENT_ONE_PACKET_SERVER_ECHO);
    return serverClass.perform_operation();

#endif

}
