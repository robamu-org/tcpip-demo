#include "demo_config.h"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>

#elif defined(__unix__)

#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif

#include <stdint.h>
#include <stdio.h>


#ifdef _WIN32
DWORD tcp_server_oneshot(LPVOID cfg) {
    // Totally not copied from https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code
    WSADATA wsa_data;
    int retval;

    SOCKET listen_socket = INVALID_SOCKET;
    SOCKET client_socket = INVALID_SOCKET;

    OneShotConfig* one_shot_config = (OneShotConfig*) cfg;
    if(one_shot_config == NULL) {
        printf("Invalid passed config handle!\n");
        return 1;
    }
    const char* server_address = one_shot_config->server_address;
    const char* server_port = one_shot_config->server_port;

    struct addrinfo *addr_result = NULL;
    struct addrinfo hints;

    uint8_t rec_buf[BUFFER_SIZES];
    int iSendResult;
    int recvbuflen = sizeof(rec_buf);

    // Initialize Winsock
    retval = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if (retval != 0) {
        printf("WSAStartup failed with error: %d\n", retval);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    if(server_address == "" || server_address == "any") {
        retval = getaddrinfo(NULL, server_port, &hints, &addr_result);
    }
    else {
        retval = getaddrinfo(server_address, server_port, &hints, &addr_result);
    }

    if (retval != 0) {
        printf("getaddrinfo failed with error: %d\n", retval);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    listen_socket = socket(addr_result->ai_family, addr_result->ai_socktype,
            addr_result->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(addr_result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    retval = bind( listen_socket, addr_result->ai_addr, (int)addr_result->ai_addrlen);
    if (retval == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(addr_result);
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(addr_result);

    retval = listen(listen_socket, SOMAXCONN);
    if (retval == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    client_socket = accept(listen_socket, NULL, NULL);
    if (client_socket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(listen_socket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    closesocket(listen_socket);

    // Receive until the peer shuts down the connection
    do {

        retval = recv(client_socket, rec_buf, recvbuflen, 0);
        if (retval > 0) {
            printf(SRV_CLR "Server: Bytes Received: %d\n", retval);
            printf(SRV_CLR "Server: Received string: %s\n", rec_buf);

            // Echo the buffer back to the sender
            iSendResult = send( client_socket, rec_buf, retval, 0 );
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(client_socket);
                WSACleanup();
                return 1;
            }
            printf(SRV_CLR "Server: Bytes echoed back: %d\n", iSendResult);
        }
        else if (retval == 0)
            printf(SRV_CLR "Server: Client closed connection...\n");
        else  {
            printf("Server: recv failed with error: %d\n", WSAGetLastError());
            closesocket(client_socket);
            WSACleanup();
            return 1;
        }

    } while (retval > 0);

    printf(SRV_CLR "Server: Shutting down\n");
    // shutdown the connection since we're done
    retval = shutdown(client_socket, SD_SEND);
    if (retval == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(client_socket);
    WSACleanup();

    return 0;
}
#elif defined(__unix__)

int tcp_server_oneshot_main(void* args);

void* tcp_server_oneshot(void* args) {
    tcp_server_oneshot_main(args);
    return NULL;
}

int tcp_server_oneshot_main(void* args) {
    // Based on https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code
    int retval = 0;
    int send_result = 0;
    int listen_socket = 0;
    int client_socket = 0;

    struct addrinfo *result = NULL;
    struct addrinfo hints = {};

    OneShotConfig* one_shot_config = (OneShotConfig*) args;
    if(one_shot_config == NULL) {
        printf("Invalid passed config handle!\n");
        return 1;
    }
    const char* server_address = one_shot_config->server_address;
    const char* server_port = one_shot_config->server_port;

    uint8_t rec_buf[BUFFER_SIZES];
    int recvbuflen = sizeof(rec_buf);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    /* This is required on Linux, also see
    https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
    */
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    if(server_address == "" || server_address == "any") {
        retval = getaddrinfo(NULL, server_port, &hints, &result);
    }
    else {
        retval = getaddrinfo(server_address, server_port, &hints, &result);
    }

    if (retval != 0) {
        printf("getaddrinfo for address %s failed with error: %d\n", server_address, retval);
        return 1;
    }

    // Create a SOCKET for connecting to server
    listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listen_socket == 0) {
        printf("Server: socket failed with error: %d\n", errno);
        freeaddrinfo(result);
        return 1;
    }

    // Setup the TCP listening socket
    retval = bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
    if (retval != 0) {
        printf("bind failed with error: %d\n", errno);
        freeaddrinfo(result);
        close(listen_socket);
        return 1;
    }

    freeaddrinfo(result);

    retval = listen(listen_socket, SOMAXCONN);
    if (retval != 0) {
        printf("listen failed with error: %d\n", errno);
        close(listen_socket);
        return 1;
    }

    // Accept a client socket
    client_socket = accept(listen_socket, NULL, NULL);
    if (client_socket == 0) {
        printf("accept failed with error: %d\n", errno);
        close(listen_socket);
        return 1;
    }

    // No longer need server socket
    close(listen_socket);

    // Receive until the peer shuts down the connection
    do {

        retval = recv(client_socket, rec_buf, recvbuflen, 0);
        if (retval > 0) {
            printf(SRV_CLR "Server: Bytes received: %d\n", retval);
            printf(SRV_CLR "Server: Received string: %s\n", rec_buf);
            printf(SRV_CLR "Server: Echoing back string\n");
            // Echo the buffer back to the sender
            send_result = send(client_socket, rec_buf, retval, 0 );
            if (send_result < 0) {
                printf("send failed with error: %d\n", errno);
                close(client_socket);
                return 1;
            }
            printf(SRV_CLR "Server: Bytes echoed back: %d\n", send_result);
        }
        else if (retval == 0)
            printf(SRV_CLR "Server: Client closed connection\n");
        else  {
            printf("Server: recv failed with error: %d\n", errno);
            close(client_socket);
            return 1;
        }

    } while (retval > 0);

    // shutdown the connection since we're done
    retval = shutdown(client_socket, SHUT_WR);
    if (retval != 0) {
        printf("shutdown failed with error: %d\n", errno);
        close(client_socket);
        return 1;
    }

    // cleanup
    close(client_socket);

    return 0;
}

#endif /* UNIX */
