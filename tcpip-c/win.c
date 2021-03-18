#include "main.h"

#include <windows.h>
#include <processthreadsapi.h>
#include <synchapi.h>

#include <stdio.h>
#include <stddef.h>

DWORD tcp_client_oneshot(LPVOID cfg);
DWORD tcp_server_oneshot(LPVOID cfg);

int start_demo(int protocol, int demo_type, int demo_mode) {
    HANDLE thread_handle_client = NULL;
    HANDLE thread_handle_server = NULL;
    if(protocol == TCPIP_PROT_0_TCP) {
        /* Start the server */
        if(demo_type == TCPIP_TP_1_SERVER_ONLY || demo_type == TCPIP_TP_0_CLIENT_AND_SERVER) {
            thread_handle_server =  CreateThread(NULL, 0, tcp_server_oneshot,
                    (void*) &one_shot_config, 0, NULL);
            if(thread_handle_server == NULL) {
                printf("Windows thread creation failed with error %d!\n", GetLastError());
                return 1;
            }
        }
        Sleep(1);
        /* Start the client */
        if(demo_type == TCPIP_TP_2_CLIENT_ONLY || demo_type == TCPIP_TP_0_CLIENT_AND_SERVER) {
            thread_handle_client =  CreateThread(NULL, 0, tcp_client_oneshot,
                    (void*) &one_shot_config, 0, NULL);
            if(thread_handle_client == NULL) {
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
