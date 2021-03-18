#include "demo_config.h"
#include "main.h"

#include <unistd.h>
#include <pthread.h>

#include <stdio.h>

void* tcp_client_oneshot(void* args);
void* tcp_server_oneshot(void* args);

int start_demo(int protocol, int demo_type, int demo_mode) {
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
