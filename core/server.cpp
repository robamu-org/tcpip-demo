#include "config.h"

#include <iostream>

int server_oneshot_win32();

int server_application() {
    std::cout << "Server application startup.." << std::endl;

#if DEMO_TYPE == SIMPLE_ONE_SHOT_DEMO
    /* Simple one-shot demo, no classes or any fancy stuff */
#ifdef WIN32
    return server_oneshot_win32();
#elif UNIX
    return 0;
#endif

#else

    return 0;

#endif

}
