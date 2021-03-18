#include <demo_config.h>
#include <utility.h>
#include <socket_inc.h>

#include <stdio.h>
#include <stdint.h>

int udp_server_oneshot_generic(void* args);

#ifdef _WIN32
DWORD udp_server_oneshot(LPVOID args) {
    return udp_server_oneshot_generic(args);
}
#else
void* udp_server_oneshot(void* args) {
    udp_server_oneshot_generic(args);
    return NULL;
}
#endif

int udp_server_oneshot_generic(void* args) {
    const char* server_address = NULL;
    const char* server_port = NULL;
    int retval = setup_sockets(args, &server_address, &server_port);
    if(retval != 0) {
        return retval;
    }

    struct addrinfo *result = NULL;
    struct addrinfo hints = {0};

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
        printf("udp_server_oneshot: getaddrinfo failed with error: %d\n", get_last_socket_error());
        cleanup_sockets(NULL, 0);
        return 1;
    }
    /* Create a socket for connecting to server */
    server_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (server_socket < 0) {
        printf("udp_server_oneshot: socket failed with error: %d\n", get_last_socket_error());
        freeaddrinfo(result);
        cleanup_sockets(NULL, 0);
        return 1;
    }

    /* Setup the UDP server socket */
    retval = bind(server_socket, result->ai_addr, (int)result->ai_addrlen);
    if (retval != 0) {
        printf("udp_server_oneshot: bind failed with error: %d\n", retval);
        freeaddrinfo(result);
        cleanup_sockets(NULL, 0);
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
        printf("Server: recvfrom failed with error %d\n", get_last_socket_error());
        cleanup_sockets(&server_socket, 1);
        return 1;
    }

    cleanup_sockets(&server_socket, 1);
    return 0;
}
