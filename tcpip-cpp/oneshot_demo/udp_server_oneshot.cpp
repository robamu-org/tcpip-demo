#include <demo_config.h>
#include <core/tcpipCommon.h>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>

#elif defined(__unix__)

#include <netdb.h>
#include <unistd.h>

#endif

#include <vector>
#include <iostream>

int udp_server_oneshot(std::string server_address) {
    struct addrinfo *result = nullptr;
    struct addrinfo hints = {};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;

    std::vector<uint8_t> rec_buf(tcpip::BUFFER_SIZES);
    socket_t server_socket = INVALID_SOCKET;
    int udp_flags = 0;

    /* Resolve the server address and port */
    int retval = 0;
    if(server_address == "any" or server_address == "") {
        retval = getaddrinfo(nullptr, tcpip::SERVER_PORT, &hints, &result);
    }
    else {
        retval = getaddrinfo(server_address.c_str(), tcpip::SERVER_PORT, &hints, &result);
    }

    if (retval != 0) {
        std::cerr << "udp_server_oneshot: getaddrinfo failed with error: " <<
                retval << std::endl;
        return 1;
    }
    /* Create a socket for connecting to server */
    server_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (server_socket < 0) {
        std::cerr << "udp_server_oneshot: socket failed with error: " <<
                errno << std::endl;
        freeaddrinfo(result);
        return 1;
    }

    /* Setup the UDP server socket */
    retval = bind(server_socket, result->ai_addr, (int)result->ai_addrlen);
    if (retval != 0) {
        std::cerr << "udp_server_oneshot: bind failed with error: " <<
                tcpip::get_last_error() << std::endl;
        freeaddrinfo(result);
        return 1;
    }

    freeaddrinfo(result);

    struct sockaddr sender;
    int sender_sock_len = sizeof(sender);

    retval = recvfrom(
            server_socket,
            reinterpret_cast<char*>(rec_buf.data()),
            rec_buf.capacity() - 1,
            udp_flags,
            &sender,
            &sender_sock_len
    );
    if(retval > 0) {
        std::cout << "Server: Packet with " << retval << " bytes received" << std::endl;
    }
    else if(retval == 0) {
        std::cout << "Server: Empty packet received" << std::endl;
    }
    else {
        int error = tcpip::get_last_error();
        std::cerr << "Server: recvfrom failed with error " << error << std::endl;
    }

    return 0;
}
