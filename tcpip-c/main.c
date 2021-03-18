#include "utility.h"
#include "demo_config.h"
#include <stdio.h>

OneShotConfig one_shot_config;


/**
 * @brief   TCP/IP C demo entry point.
 * @details
 * This program can be configured by tweaking the demo_config.h file.
 * @author  R. Mueller
 * @return
 */
int main() {
    char os_name[20];
    get_os_name(os_name, sizeof(os_name));
    printf("Starting TCP/IP Server/Client C demo on %s..\n", os_name);

    one_shot_config.server_address = SERVER_ADDRESS;
    one_shot_config.server_port = SERVER_PORT;

    int protocol = TCPIP_DEMO_PROT;
    int demo_type = TCPIP_DEMO_TYPE;
    int demo_mode = TCPIP_DEMO_MODE;
    return start_demo(protocol, demo_type, demo_mode);
}
