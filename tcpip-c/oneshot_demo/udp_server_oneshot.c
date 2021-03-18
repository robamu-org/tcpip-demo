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


DWORD udp_server_oneshot(LPVOID cfg) {
    WSADATA wsaData;
    /* Initialize Winsock */
    int retval = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (retval!= 0) {
        printf("WSAStartup failed with error: %d\n", retval);
        return 1;
    }

    OneShotConfig* one_shot_config = (OneShotConfig*) cfg;
    if(one_shot_config == NULL) {
        printf("Invalid passed config handle!\n");
        return 1;
    }
    const char* server_address = one_shot_config->server_address;
    const char* server_port = one_shot_config->server_port;

    struct addrinfo *result = NULL;
    struct addrinfo hints = {};

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    uint8_t rec_buf[BUFFER_SIZES];
    socket_t server_socket = INVALID_SOCKET;
    int udp_flags = 0;

    /* Resolve the server address and port */
    if(server_address == "any" || server_address == "") {
        retval = getaddrinfo(NULL, server_port, &hints, &result);
    }
    else {
        retval = getaddrinfo(server_address, server_port, &hints, &result);
    }

    if (retval != 0) {
        printf("udp_server_oneshot: getaddrinfo failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    /* Create a socket for connecting to server */
    server_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (server_socket < 0) {
        printf("udp_server_oneshot: socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    /* Setup the UDP server socket */
    retval = bind(server_socket, result->ai_addr, (int)result->ai_addrlen);
    if (retval != 0) {
        printf("udp_server_oneshot: bind failed with error: %d\n", retval);
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    struct sockaddr sender;
    int sender_sock_len = sizeof(sender);

    retval = recvfrom(
            server_socket,
            rec_buf,
            sizeof(rec_buf),
            udp_flags,
            &sender,
            &sender_sock_len
    );
    if(retval > 0) {
        printf(SRV_CLR "Server: Packet with %d bytes received\n", retval);
        int send_flags = 0;
        int send_ret = sendto(
                server_socket,
                rec_buf,
                retval,
                send_flags,
                &sender,
                sender_sock_len
        );
        if(send_ret > 0) {
            printf(SRV_CLR "Server: Echoed back packet successfully\n");
        }
    }
    else if(retval == 0) {
        printf("Server: Empty packet received\n");
    }
    else {
        printf("Server: recvfrom failed with error %d", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    return 0;
}
