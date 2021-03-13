#include "demo_config.h"

#include <iostream>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>

#elif defined(__unix__)

#include <netdb.h>
#include <unistd.h>

#endif

#include <vector>


#ifdef WIN32
int server_oneshot() {
    /* Totally not copied from https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code */
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    std::vector<uint8_t> reception_buffer(RECEIVE_BUFFER_SIZE);
    int iSendResult;
    int recvbuflen = RECEIVE_BUFFER_SIZE;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(nullptr, tcpip::SERVER_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    // Receive until the peer shuts down the connection
    do {

        iResult = recv(ClientSocket, reinterpret_cast<char*>(reception_buffer.data()), recvbuflen, 0);
        if (iResult > 0) {
            printf("Server: Bytes Received: %d\n", iResult);

            // Echo the buffer back to the sender
            iSendResult = send( ClientSocket, reinterpret_cast<char*>(reception_buffer.data()), iResult, 0 );
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Server: Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("Server: Connection closing...\n");
        else  {
            printf("Server: recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}
#elif defined(__unix__)

int server_oneshot() {
    /* Based on https://docs.microsoft.com/en-us/windows/win32/winsock/complete-server-code */
    int retval = 0;
    int send_result = 0;
    int listen_socket = 0;
    int client_socket = 0;

    struct addrinfo *result = NULL;
    struct addrinfo hints = {};

    std::vector<uint8_t> reception_buffer(tcpip::BUFFER_SIZES);
    int recvbuflen = reception_buffer.size();

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    /* This is required on Linux, also see
    https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
    */
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    retval = getaddrinfo(nullptr, tcpip::SERVER_PORT, &hints, &result);
    if (retval != 0) {
        printf("getaddrinfo failed with error: %d\n", retval);
        return 1;
    }

    // Create a SOCKET for connecting to server
    listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listen_socket == 0) {
        printf("socket failed with error: %d\n", errno);
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

        retval = recv(client_socket, reinterpret_cast<char*>(reception_buffer.data()), recvbuflen, 0);
        if (retval > 0) {
            printf("Server: Bytes received: %d\n", retval);
            printf("Server: Received string: %s\n", reception_buffer.data());
            printf("Server: Echoing back string\n");
            // Echo the buffer back to the sender
            send_result = send(client_socket, reinterpret_cast<char*>(reception_buffer.data()), retval, 0 );
            if (send_result < 0) {
                printf("send failed with error: %d\n", errno);
                close(client_socket);
                return 1;
            }
            printf("Server: Bytes echoed back: %d\n", send_result);
        }
        else if (retval == 0)
            printf("Server: Connection closing...\n");
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
