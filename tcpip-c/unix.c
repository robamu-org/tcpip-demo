#include "demo_config.h"
#include "main.h"

#include <unistd.h>
#include <pthread.h>

#include <stdio.h>

void* tcp_client_oneshot(void* args);
void* tcp_server_oneshot(void* args);
void* udp_client_oneshot(void* args);
void* udp_server_oneshot(void* args);

int start_demo(int protocol, int demo_type, int demo_mode) {
    pthread_t client_handle = 0;
    pthread_t server_handle = 0;
    /* Not used for now but created if needed in the future */
    pthread_attr_t client_attrs;
    pthread_attr_t server_attrs;
    void* (*exec_func_client) (void*) = NULL;
    void* (*exec_func_server) (void*) = NULL;
    void* args_client = &one_shot_config;
    void* args_server = &one_shot_config;
    int retval = 0;
    if(protocol == TCPIP_PROT_0_TCP) {
        if(demo_type == TCPIP_TP_0_CLIENT_AND_SERVER || demo_type == TCPIP_TP_1_SERVER_ONLY) {
            exec_func_server = tcp_server_oneshot;
        }
        if(demo_type == TCPIP_TP_0_CLIENT_AND_SERVER || demo_type == TCPIP_TP_2_CLIENT_ONLY) {
            exec_func_client = tcp_client_oneshot;
        }
    }
    else if(protocol == TCPIP_PROT_1_UDP) {
        if(demo_type == TCPIP_TP_0_CLIENT_AND_SERVER || demo_type == TCPIP_TP_1_SERVER_ONLY) {
            exec_func_server = udp_server_oneshot;
        }
        if(demo_type == TCPIP_TP_0_CLIENT_AND_SERVER || demo_type == TCPIP_TP_2_CLIENT_ONLY) {
            exec_func_client = udp_client_oneshot;
        }
    }

    if(exec_func_server != NULL) {
        retval = pthread_create(&server_handle, NULL, exec_func_server, args_server);
        if(retval != 0) {
            printf("Unix thread creation failed with error %d!\n", retval);
            return 1;
        }
    }
    if(exec_func_client != NULL) {
        if(exec_func_server != NULL) {
            int sleep_dur_micros = 1 * 1000;
            usleep(sleep_dur_micros);
        }
        retval = pthread_create(&client_handle, NULL, exec_func_client, args_client);
        if(retval != 0) {
            printf("Unix thread creation failed with error %d!\n", retval);
            return 1;
        }
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
