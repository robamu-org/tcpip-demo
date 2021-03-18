#include "utility.h"
#include "demo_config.h"
#include <stdio.h>

#ifdef _WIN32

#include <windows.h>
#include <processthreadsapi.h>

#else
#endif

OneShotConfig one_shot_config;

#ifdef _WIN32
DWORD tcp_client_oneshot(LPVOID cfg);
DWORD tcp_server_oneshot(LPVOID cfg);
int start_win_demo(int protocol, int demo_type, int demo_mode);
#else
#endif



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
#ifdef _WIN32
    return start_win_demo(protocol, demo_type, demo_mode);
#else
    printf("Demo not implemented for this OS yet\n");
    return 0;
#endif
}

#ifdef _WIN32
int start_win_demo(int protocol, int demo_type, int demo_mode) {
    HANDLE thread_handle_client = NULL;
    HANDLE thread_handle_server = NULL;
    if(protocol == TCPIP_PROT_0_TCP) {
        if(demo_type == TCPIP_TP_2_CLIENT_ONLY || demo_type == TCPIP_TP_0_CLIENT_AND_SERVER) {
            thread_handle_client =  CreateThread(NULL, 0, tcp_client_oneshot,
                    (void*) &one_shot_config, 0, NULL);
            if(thread_handle_client == NULL) {
                printf("Windows thread creation failed with error %d!\n", GetLastError());
                return 1;
            }
        }
        if(demo_type == TCPIP_TP_1_SERVER_ONLY || demo_type == TCPIP_TP_0_CLIENT_AND_SERVER) {
            thread_handle_server =  CreateThread(NULL, 0, tcp_server_oneshot,
                    (void*) &one_shot_config, 0, NULL);
            if(thread_handle_server == NULL) {
                printf("Windows thread creation failed with error %d!\n", GetLastError());
                return 1;
            }
        }
    }
    else {
        printf("UDP protocol not implemented yet!\n");
        return 0;
    }

    if(thread_handle_client != NULL) {
        WaitForSingleObject(thread_handle_client, INFINITE);
    }
    if(thread_handle_server != NULL) {
        WaitForSingleObject(thread_handle_server, INFINITE);
    }
    return 0;
}

#endif
