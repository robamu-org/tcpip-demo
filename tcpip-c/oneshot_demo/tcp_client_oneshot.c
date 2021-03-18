#include "demo_config.h"

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

#ifdef _WIN32
DWORD tcp_client_oneshot(LPVOID cfg) {
    // Totally not copied from https://docs.microsoft.com/en-us/windows/win32/winsock/complete-client-code
    WSADATA wsa_data;
    socket_t connect_socket = INVALID_SOCKET;

    OneShotConfig* one_shot_config = (OneShotConfig*) cfg;
    if(one_shot_config == NULL) {
        printf("Invalid passed config handle!\n");
        return 1;
    }
    const char* server_address = one_shot_config->server_address;
    const char* server_port = one_shot_config->server_port;

    struct addrinfo *result = NULL, *ptr = NULL, hints;
    const char *sendbuf = "this is a test";
    uint8_t rec_buf[BUFFER_SIZES];
    int iResult;
    int recvbuflen = sizeof(rec_buf);

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsa_data);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(server_address, server_port, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *)ptr->ai_addr;
        char *ip = inet_ntoa(addr_in->sin_addr);
        printf("Client: Attempting connection to address %s\n", ip);
        // Create a SOCKET for connecting to server
        connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connect_socket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(connect_socket);
            connect_socket = INVALID_SOCKET;
            continue;
        }

        printf("Client: Connected successfully to %s\n", ip);
        break;
    }

    freeaddrinfo(result);

    if (connect_socket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    iResult = send( connect_socket, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(connect_socket);
        WSACleanup();
        return 1;
    }

    printf(CL_CLR "Client: Bytes Sent: %ld\n", iResult);
    printf(CL_CLR "Client: Sent string: %s\n", sendbuf);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(connect_socket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(connect_socket);
        WSACleanup();
        return 1;
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(connect_socket, rec_buf, recvbuflen, 0);
        if ( iResult > 0 ) {
            printf(CL_CLR "Client: Bytes Received: %d\n", iResult);
            printf(CL_CLR "Client: Received string: %s\n", rec_buf);
        }
        else if ( iResult == 0 )
            printf(CL_CLR "Client: Server closed connection\n");
        else
            printf(CL_CLR "Client: recv failed with error: %d\n", WSAGetLastError());

    } while( iResult > 0 );

    // cleanup
    closesocket(connect_socket);
    WSACleanup();

    return 0;
}

#elif defined(__unix__)

int tcp_client_oneshot_main(void* args);

void* tcp_client_oneshot(void* args) {
    tcp_client_oneshot_main(args);
    return NULL;
}

int tcp_client_oneshot_main(void* args) {
    /* Based on https://docs.microsoft.com/en-us/windows/win32/winsock/complete-client-code */
    int connect_socket = 0;
    struct addrinfo *result = NULL;
    struct addrinfo hints = {};

    const char *sendbuf = "this is a test";
    OneShotConfig* one_shot_config = (OneShotConfig*) args;
    if(one_shot_config == NULL) {
        printf("Invalid passed config handle!\n");
        return 1;
    }
    const char* server_address = one_shot_config->server_address;
    const char* server_port = one_shot_config->server_port;
    uint8_t rec_buf[BUFFER_SIZES];
    int retval;
    int recvbuflen = sizeof(rec_buf);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    retval = getaddrinfo(server_address, server_port, &hints, &result);
    if (retval != 0) {
        printf("getaddrinfo failed with error: %d\n", retval);
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(struct addrinfo* ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *)ptr->ai_addr;
        char *ip = inet_ntoa(addr_in->sin_addr);
        printf(ANSI_COLOR_CYAN "Client: Attempting connection to address %s failed!\n", ip);
        // Create a SOCKET for connecting to server
        connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connect_socket == 0) {
            printf("Client: socket failed with error: %d\n", errno);
            return 1;
        }

        // Connect to server.
        retval = connect(connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (retval != 0) {
            close(connect_socket);
            connect_socket = -1;
            continue;
        }

        printf(ANSI_COLOR_CYAN "Client: Connected successfully to %s\n", ip);
        break;
    }

    freeaddrinfo(result);

    if (connect_socket < 0) {
        printf("Client: Unable to connect to server!\n");
        return 1;
    }

    printf(ANSI_COLOR_CYAN "Client: Sending string: %s\n", sendbuf);
    // Send an initial buffer
    retval = send(connect_socket, sendbuf, (int)strlen(sendbuf), 0 );
    if (retval < 0) {
        printf("send failed with error: %d\n", errno);
        close(connect_socket);
        return 1;
    }

    printf(ANSI_COLOR_CYAN "Client: Bytes sent: %d\n", retval);

    // shutdown the connection since no more data will be sent
    retval = shutdown(connect_socket, SHUT_WR);
    if (retval != 0) {
        printf("shutdown failed with error: %d\n", errno);
        close(connect_socket);
        return 1;
    }

    // Receive until the peer closes the connection
    do {

        retval = recv(connect_socket, rec_buf, recvbuflen, 0);
        if (retval > 0) {
            printf(ANSI_COLOR_CYAN "Client: Bytes received: %d\n", retval);
            printf(ANSI_COLOR_CYAN "Client: String received back: %s\n", rec_buf);
        }
        else if (retval == 0) {
            printf(ANSI_COLOR_CYAN "Client: Connection closed\n");
        }
        else {
            printf(ANSI_COLOR_CYAN "Client: recv failed with error: %d\n", errno);
        }

    } while(retval > 0);

    // cleanup
    close(connect_socket);

    return 0;
}


#endif /* UNIX */
