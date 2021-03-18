#include "main.h"

#include <windows.h>
#include <processthreadsapi.h>
#include <synchapi.h>

#include <stdio.h>
#include <stddef.h>

DWORD tcp_client_oneshot(LPVOID cfg);
DWORD tcp_server_oneshot(LPVOID cfg);
DWORD udp_client_oneshot(LPVOID cfg);
DWORD udp_server_oneshot(LPVOID cfg);

int start_demo(int protocol, int demo_type, int demo_mode) {
    HANDLE thread_handle_client = NULL;
    HANDLE thread_handle_server = NULL;
    DWORD (* exec_func_client) (LPVOID) = NULL;
    DWORD (* exec_func_server) (LPVOID) = NULL;
    LPVOID client_args = (void*) &one_shot_config;
    LPVOID server_args = (void*) &one_shot_config;
    if(protocol == TCPIP_PROT_0_TCP) {
        if(demo_type == TCPIP_TP_1_SERVER_ONLY || demo_type == TCPIP_TP_0_CLIENT_AND_SERVER) {
            exec_func_server = tcp_server_oneshot;

        }
        if(demo_type == TCPIP_TP_2_CLIENT_ONLY || demo_type == TCPIP_TP_0_CLIENT_AND_SERVER) {
            exec_func_client = tcp_client_oneshot;
        }

    }
    else if(protocol == TCPIP_PROT_1_UDP) {
        if(demo_type == TCPIP_TP_1_SERVER_ONLY || demo_type == TCPIP_TP_0_CLIENT_AND_SERVER) {
            exec_func_server = udp_server_oneshot;

        }
        if(demo_type == TCPIP_TP_2_CLIENT_ONLY || demo_type == TCPIP_TP_0_CLIENT_AND_SERVER) {
            exec_func_client = udp_client_oneshot;
        }
    }

    /* Start the server */
    if(exec_func_server != NULL) {
        thread_handle_server =  CreateThread(NULL, 0, exec_func_server, server_args, 0, NULL);
        if(thread_handle_server == NULL) {
            printf("Windows server thread creation failed with error %d!\n", GetLastError());
            return 1;
        }

    }

    /* Start the client */
    if(exec_func_client != NULL) {
        if(exec_func_server != NULL) {
            Sleep(1);
        }
        thread_handle_client =  CreateThread(NULL, 0, exec_func_client, client_args, 0, NULL);
        if(thread_handle_client == NULL) {
            printf("Windows client thread creation failed with error %d!\n", GetLastError());
            return 1;
        }
    }

    if(thread_handle_client != NULL) {
        WaitForSingleObject(thread_handle_client, INFINITE);
    }
    if(thread_handle_server != NULL) {
        WaitForSingleObject(thread_handle_server, INFINITE);
    }
    return 0;
}
