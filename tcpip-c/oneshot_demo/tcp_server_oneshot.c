#include "demo_config.h"
#include <utility.h>
#include <socket_inc.h>

#include <stdint.h>
#include <stdio.h>

int tcp_server_oneshot_generic(void* args);

#ifdef _WIN32
DWORD tcp_server_oneshot(LPVOID cfg) {
    return tcp_server_oneshot_generic(cfg);
}
#elif defined(__unix__)
void * tcp_server_oneshot(void* args) {
    tcp_server_oneshot_generic(args);
    return NULL;
}
#endif

int tcp_server_oneshot_generic(void* args) {
    // Totally not copied from https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code
    int retval;

    const char* server_address = NULL;
    const char* server_port = NULL;
    setup_sockets(args, &server_address, &server_port);

    socket_t listen_socket = INVALID_SOCKET;
    socket_t client_socket = INVALID_SOCKET;

    struct addrinfo *addr_result = NULL;
    struct addrinfo hints = { 0 };

    uint8_t rec_buf[BUFFER_SIZES];
    int iSendResult;
    int recvbuflen = sizeof(rec_buf);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    if(server_address == "" || server_address == "any") {
        /*
        See: https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
        and: https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo
         */
        hints.ai_flags = AI_PASSIVE;
        retval = getaddrinfo(NULL, server_port, &hints, &addr_result);
    }
    else {
        retval = getaddrinfo(server_address, server_port, &hints, &addr_result);
    }

    if (retval != 0) {
        printf("getaddrinfo failed with error: %d\n", retval);
        cleanup_sockets(NULL, 0);
        return 1;
    }

    // Create a SOCKET for connecting to server
    listen_socket = socket(addr_result->ai_family, addr_result->ai_socktype,
            addr_result->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        printf("socket failed with error: %d\n", get_last_socket_error());
        freeaddrinfo(addr_result);
        cleanup_sockets(NULL, 0);
        return 1;
    }

    // Setup the TCP listening socket
    retval = bind( listen_socket, addr_result->ai_addr, (int)addr_result->ai_addrlen);
    if (retval == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", get_last_socket_error());
        freeaddrinfo(addr_result);
        cleanup_sockets(&listen_socket, 1);
        return 1;
    }

    freeaddrinfo(addr_result);

    retval = listen(listen_socket, SOMAXCONN);
    if (retval == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", get_last_socket_error());
        cleanup_sockets(&listen_socket, 1);
        return 1;
    }

    // Accept a client socket
    client_socket = accept(listen_socket, NULL, NULL);
    if (client_socket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", get_last_socket_error());
        cleanup_sockets(&listen_socket, 1);
        return 1;
    }

    // No longer need server socket
    close_socket(listen_socket);

    // Receive until the peer shuts down the connection
    do {

        retval = recv(client_socket, rec_buf, recvbuflen, 0);
        if (retval > 0) {
            printf(SRV_CLR "Server: Bytes Received: %d\n", retval);
            printf(SRV_CLR "Server: Received string: %s\n", rec_buf);

            // Echo the buffer back to the sender
            iSendResult = send( client_socket, rec_buf, retval, 0 );
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", get_last_socket_error());
                cleanup_sockets(&client_socket, 1);
                return 1;
            }
            printf(SRV_CLR "Server: Bytes echoed back: %d\n", iSendResult);
        }
        else if (retval == 0)
            printf(SRV_CLR "Server: Client closed connection...\n");
        else  {
            printf("Server: recv failed with error: %d\n", get_last_socket_error());
            cleanup_sockets(&client_socket, 1);
            return 1;
        }

    } while (retval > 0);

    printf(SRV_CLR "Server: Shutting down\n");
    // shutdown the connection since we're done
    retval = shutdown(client_socket, SHUT_SEND);
    if (retval == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", get_last_socket_error());
        cleanup_sockets(&client_socket, 1);
        return 1;
    }

    cleanup_sockets(&client_socket, 1);
    return 0;
}
