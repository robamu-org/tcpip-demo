#include <demo_config.h>
#include <utility.h>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>

#elif defined(__unix__)

#include <netdb.h>
#include <unistd.h>

#endif

#include <stdio.h>
#include <stdint.h>

int udp_client_oneshot_generic(void* args);

#ifdef _WIN32
DWORD udp_client_oneshot(LPVOID args) {

}
#elif defined(__unix__)
void* udp_client_oneshot(void* args) {
    udp_client_oneshot_generic(args);
    return NULL;
}
#endif

int udp_client_oneshot_generic(void* args) {
    const char* server_address = NULL;
    const char* server_port = NULL;
    setup_sockets(args, &server_address, &server_port);

    uint8_t rec_buf[BUFFER_SIZES];

    struct addrinfo *result = NULL;
    struct addrinfo hints = {0};

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    socket_t server_socket = INVALID_SOCKET;
    int retval = 0;

    /* Resolve the server address and port */
    if(server_address == "any" || server_address == "") {
        retval = getaddrinfo(NULL, server_port, &hints, &result);
    }
    else {
        retval = getaddrinfo(server_address, server_port,  &hints, &result);
    }

    if (retval != 0) {
        printf("udp_client_oneshot: getaddrinfo failed with error: %d\n", retval);
        cleanup_sockets(NULL, 0);
        return 1;
    }
    /* Create a socket for connecting to server */
    server_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (server_socket < 0) {
        printf("udp_client_oneshot: socket failed with error: %d\n", get_last_socket_error());
        freeaddrinfo(result);
        cleanup_sockets(NULL, 0);
        return 1;
    }

    const char string[] = "This is a UDP test";
    printf(CL_CLR "Client: Sending string with %zu bytes: %s\n", sizeof(string), string);
    retval = sendto(server_socket, string, sizeof(string), 0, result->ai_addr,
            result->ai_addrlen);
    if(retval > 0) {
        printf("Client: Sent string with %zu bytes successfully\n", sizeof(string));
        int recv_flags = 0;
        retval = recv(server_socket,
                rec_buf,
                sizeof(rec_buf) - 1,
                recv_flags
        );
        if(retval < 0) {
            printf("udp_client_oneshot: recv failed with error: %d\n", get_last_socket_error());
            cleanup_sockets(&server_socket, 1);
            return 1;
        }
        rec_buf[retval] = '\0';
        printf(CL_CLR "Client: Reply with %d bytes received: %s\n", retval, rec_buf);
    }
    else {
        printf("udp_client_oneshot: sendto failed with error %d\n", get_last_socket_error());
        cleanup_sockets(&server_socket, 1);
        return 1;
    }
    return 0;
    cleanup_sockets(&server_socket, 1);
}
