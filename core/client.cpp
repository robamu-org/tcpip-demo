#include "demo_config.h"
#include <iostream>

int client_oneshot_win32();

int client_application() {
	std::cout << "Client application startup.." << std::endl;
    
#if TCPIP_DEMO_TYPE == TCPIP_SIMPLE_ONE_SHOT_DEMO
    /* Simple one-shot demo, no classes or any fancy stuff */
#ifdef WIN32
    return client_oneshot_win32();
#elif UNIX
    return 0;
#endif

#elif TCPIP_DEMO_TYPE == TCPIP_SERVER_REPLYING_DEMO

    return 0;

#endif

}
