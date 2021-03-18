#include <demo_config.h>

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

#ifdef _WIN32
DWORD udp_client_oneshot(LPVOID cfg) {
    WSADATA wsaData;

    OneShotConfig* one_shot_config = (OneShotConfig*) cfg;
    if(one_shot_config == NULL) {
        printf("Invalid passed config handle!\n");
        return 1;
    }
    const char* server_address = one_shot_config->server_address;
    const char* server_port = one_shot_config->server_port;

    /* Initialize Winsock */
    int retval = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (retval!= 0) {
        printf("WSAStartup failed with error: %d\n", retval);
        return 1;
    }

    uint8_t rec_buf[BUFFER_SIZES];

    struct addrinfo *result = NULL;
    struct addrinfo hints = {0};

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    socket_t server_socket = INVALID_SOCKET;

    /* Resolve the server address and port */
    if(server_address == "any" || server_address == "") {
        retval = getaddrinfo(NULL, server_port, &hints, &result);
    }
    else {
        retval = getaddrinfo(server_address, server_port,  &hints, &result);
    }

    if (retval != 0) {
        printf("udp_client_oneshot: getaddrinfo failed with error: %d\n", retval);
        WSACleanup();
        return 1;
    }
    /* Create a socket for connecting to server */
    server_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (server_socket < 0) {
        printf("udp_client_oneshot: socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
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
            printf("udp_client_oneshot: recv failed with error: %d", WSAGetLastError());
            WSACleanup();
            return 1;
        }
        rec_buf[retval] = '\0';
        printf(CL_CLR "Client: Reply with %d bytes received: %s", retval, rec_buf);
    }
    else {
        printf("udp_client_oneshot: sendto failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    return 0;
}
#endif
