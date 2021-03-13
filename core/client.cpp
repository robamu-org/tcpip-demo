#include "demo_config.h"
#include <iostream>

#if TCPIP_DEMO_TYPE == TCPIP_SIMPLE_ONE_SHOT_DEMO
int client_oneshot_win32();
#else
int client_oneshot_win32();
#endif

int client_application() {
	std::cout << "Client application startup.." << std::endl;
    
#if TCPIP_DEMO_TYPE == TCPIP_SIMPLE_ONE_SHOT_DEMO
    /* Simple one-shot demo, no classes or any fancy stuff */
#ifdef WIN32
    return client_oneshot_win32();
#elif UNIX
    return 0;
#endif

#elif TCPIP_DEMO_TYPE == TCPIP_CLASS_ONE_SHOT_DEMO

#ifdef WIN32
    return client_oneshot_win32();
#elif UNIX
    return 0;
#endif

#elif TCPIP_DEMO_TYPE == TCPIP_SERVER_REPLYING_DEMO

    return 0;

#else

    std::cout << "client_application: Unhandled client code!" << std::endl;
    return 0;

#endif

}
