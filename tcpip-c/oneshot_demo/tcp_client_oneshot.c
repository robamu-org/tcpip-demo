#include "demo_config.h"
#include "utility.h"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#elif defined(__unix__)

#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int tcp_client_oneshot_generic(void* args);

#ifdef _WIN32
DWORD tcp_client_oneshot(LPVOID cfg) {
    return tcp_client_oneshot_generic(cfg);
}
#elif defined(__unix__)
void * tcp_client_oneshot(void* args) {
    tcp_client_oneshot_generic(args);
    return NULL;
}
#endif

int tcp_client_oneshot_generic(void* args) {
    socket_t connect_socket = INVALID_SOCKET;
    // Totally not copied from https://docs.microsoft.com/en-us/windows/win32/winsock/complete-client-code
    const char* server_address = NULL;
    const char* server_port = NULL;
    int retval = setup_sockets(args, &server_address, &server_port);
    if (retval != 0) {
        return retval;
    }

    struct addrinfo *result = NULL;
    struct addrinfo hints = {0};
    const char *sendbuf = "this is a test";
    uint8_t rec_buf[BUFFER_SIZES];
    int recvbuflen = sizeof(rec_buf);


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    retval = getaddrinfo(server_address, server_port, &hints, &result);
    if (retval != 0) {
        printf("tcp_client_oneshot: getaddrinfo failed with error: %d\n", retval);
        cleanup_sockets(NULL, 0);
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(struct addrinfo* ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *)ptr->ai_addr;
        char *ip = inet_ntoa(addr_in->sin_addr);
        printf("Client: Attempting connection to address %s\n", ip);
        // Create a SOCKET for connecting to server
        connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connect_socket == INVALID_SOCKET) {
            printf("socket failed with error: %d\n", get_last_socket_error());
            cleanup_sockets(&connect_socket, 1);
            return 1;
        }

        // Connect to server.
        retval = connect( connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (retval < 0) {
#ifdef _WIN32
            closesocket(connect_socket);
#elif defined(__unix__)
            close(connect_socket);
#endif
            connect_socket = INVALID_SOCKET;
            continue;
        }

        printf("Client: Connected successfully to %s\n", ip);
        break;
    }

    freeaddrinfo(result);

    if (connect_socket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        cleanup_sockets(NULL, 0);
        return 1;
    }

    // Send an initial buffer
    retval = send( connect_socket, sendbuf, (int)strlen(sendbuf), 0 );
    if (retval < 0) {
        printf("tcp_client_oneshot: send failed with error: %d\n", get_last_socket_error());
        cleanup_sockets(&connect_socket, 1);
        return 1;
    }

    printf(CL_CLR "Client: Bytes Sent: %d\n", retval);
    printf(CL_CLR "Client: Sent string: %s\n", sendbuf);

    // shutdown the connection since no more data will be sent
    retval = shutdown(connect_socket, SHUT_SEND);
    if (retval == SOCKET_ERROR) {
        printf("tcp_client_oneshot: shutdown failed with error: %d\n", get_last_socket_error());
        cleanup_sockets(&connect_socket, 1);
        return 1;
    }

    // Receive until the peer closes the connection
    do {

        retval = recv(connect_socket, rec_buf, recvbuflen, 0);
        if (retval > 0) {
            printf(CL_CLR "Client: Bytes Received: %d\n", retval);
            printf(CL_CLR "Client: Received string: %s\n", rec_buf);
        }
        else if (retval == 0)
            printf(CL_CLR "Client: Server closed connection\n");
        else
            printf(CL_CLR "Client: recv failed with error: %d\n", get_last_socket_error());

    } while(retval > 0 );

    // cleanup
    cleanup_sockets(&connect_socket, 1);
    return 0;
}
