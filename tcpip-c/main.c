#include "utility.h"
#include "demo_config.h"
#include <stdio.h>

#ifdef _WIN32

#include <windows.h>
#include <processthreadsapi.h>

#elif defined(__unix__)

#include <unistd.h>
#include <pthread.h>

#endif

OneShotConfig one_shot_config;

#ifdef _WIN32
DWORD tcp_client_oneshot(LPVOID cfg);
DWORD tcp_server_oneshot(LPVOID cfg);
int start_win_demo(int protocol, int demo_type, int demo_mode);
#elif defined (__unix__)
void* tcp_client_oneshot(void* args);
void* tcp_server_oneshot(void* args);
int start_unix_demo(int protocol, int demo_type, int demo_mode);
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
#elif defined(__unix__)
    return start_unix_demo(protocol, demo_type, demo_mode);
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

#elif defined(__unix__)

int start_unix_demo(int protocol, int demo_type, int demo_mode) {
    pthread_t client_handle = 0;
    pthread_t server_handle = 0;
    /* Not used for now but created if needed in the future */
    pthread_attr_t client_attrs;
    pthread_attr_t server_attrs;
    int retval = 0;
    if(protocol == TCPIP_PROT_0_TCP) {
        if(demo_type == TCPIP_TP_1_SERVER_ONLY || demo_type == TCPIP_TP_0_CLIENT_AND_SERVER) {
            retval = pthread_create(&server_handle, NULL, tcp_server_oneshot,
                    (void*) &one_shot_config);
            if(retval != 0) {
                printf("Windows thread creation failed with error %d!\n", retval);
                return 1;
            }
        }
        int sleep_dur_micros = 2 * 1000;
        usleep(sleep_dur_micros);
        if(demo_type == TCPIP_TP_2_CLIENT_ONLY || demo_type == TCPIP_TP_0_CLIENT_AND_SERVER) {
            retval = pthread_create(&client_handle, NULL, tcp_client_oneshot,
                    (void*) &one_shot_config);
            if(retval != 0) {
                printf("Unix thread creation failed with error %d!\n", retval);
                return 1;
            }
        }
    }
    else {
        printf("UDP protocol not implemented yet!\n");
        return 0;
    }

    if(client_handle != 0) {
        int* client_retval = NULL;
        retval = pthread_join(client_handle, (void**) &client_retval);
        if(retval != 0) {
            printf("Joining client thread failed with %d\n", retval);
        }
    }
    if(server_handle != 0) {
        int* server_retval = NULL;
        retval = pthread_join(server_handle, (void**) &server_retval);
        if(retval != 0) {
            printf("Joining server thread failed with %d\n", retval);
        }
    }

    return 0;
}

#endif
