#include "utility.h"

#ifdef _WIN32
#include <winsock2.h>
#elif defined(__unix__)
#include <errno.h>
#include <unistd.h>
#endif

#include <stdio.h>

int get_os_name(char* const name_buf, size_t buf_len) {
#ifdef _WIN32
    return snprintf(name_buf, buf_len, "Windows");
#elif __APPLE__ || __MACH__
    return snprintf(name_buf, buf_len, "Mac OSX");
#elif __linux__
    return snprintf(name_buf, buf_len, "Linux");
#elif __FreeBSD__
    return snprintf(name_buf, buf_len, "FreeBSD");
#elif __unix || __unix__
    return snprintf(name_buf, buf_len, "Unix");
#else
    return snprintf(name_buf, buf_len, "Other");
#endif

}

int get_last_socket_error() {
#ifdef _WIN32
    return WSAGetLastError();
#elif defined(__unix__)
    return errno;
#endif
}

int setup_sockets(void* thread_arg, const char** server_address, const char** server_port) {
#ifdef _WIN32
    WSADATA wsa_data;
    // Initialize Winsock
    int retval = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if (retval != 0) {
        printf("WSAStartup failed with error: %d\n", retval);
        return 1;
    }
#endif

    OneShotConfig* one_shot_config = (OneShotConfig*) thread_arg;
    if(one_shot_config == NULL) {
        printf("Invalid passed config handle!\n");
        return 1;
    }
    if(server_address != NULL) {
        *server_address = one_shot_config->server_address;
    }
    if(server_port != NULL) {
        *server_port = one_shot_config->server_port;
    }
    return 0;
}

int cleanup_sockets(socket_t* socket_array, size_t array_len) {
#ifdef _WIN32
    WSACleanup();
    if(socket_array != NULL) {
        for(int idx = 0; idx < array_len; idx++) {
            closesocket(socket_array[idx]);
        }
    }
#elif defined(__unix__)
    if(socket_array != NULL) {
        for(int idx = 0; idx < array_len; idx++) {
            close(socket_array[idx]);
        }
    }
#endif
    return 0;
}

int close_socket(socket_t socket) {
#ifdef _WIN32
    return closesocket(socket);
#elif defined(__unix__)
    return close(socket);
#endif
}

