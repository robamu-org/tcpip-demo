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

int udp_client_oneshot(std::string server_address) {
    WSADATA wsaData;
    /* Initialize Winsock */
    int retval = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (retval!= 0) {
        printf("WSAStartup failed with error: %d\n", retval);
        return 1;
    }

    std::vector<uint8_t> rec_buf(tcpip::BUFFER_SIZES);

    struct addrinfo *result = nullptr;
    struct addrinfo hints = {};

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    socket_t server_socket = INVALID_SOCKET;

    /* Resolve the server address and port */
    if(server_address == "any" or server_address == "") {
        retval = getaddrinfo(nullptr, tcpip::SERVER_PORT, &hints, &result);
    }
    else {
        retval = getaddrinfo(server_address.c_str(), tcpip::SERVER_PORT,  &hints, &result);
    }

    if (retval != 0) {
        std::cerr << "udp_client_oneshot: getaddrinfo failed with error: " <<
                retval << std::endl;
        WSACleanup();
        return 1;
    }
    /* Create a socket for connecting to server */
    server_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (server_socket < 0) {
        std::cerr << "udp_client_oneshot: socket failed with error: " <<
                tcpip::get_last_error() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    std::string string = "This is a UDP test";
    std::cout << "Client: Sending string with " << string.size() << " bytes: " <<
            string << std::endl;
    retval = sendto(server_socket, string.data(), string.size(), 0, result->ai_addr,
            result->ai_addrlen);
    if(retval > 0) {
        std::cout << "Client: Sent string with " << string.size() << " bytes successfully"
                << std::endl;
        int recv_flags = 0;
        retval = recv(server_socket,
                reinterpret_cast<char*>(rec_buf.data()),
                rec_buf.capacity() - 1,
                recv_flags
        );
        if(retval < 0) {
            int error = tcpip::get_last_error();
            std::cerr << "udp_client_oneshot: recv failed with error: " << error << std::endl;
            WSACleanup();
            return 1;
        }
        rec_buf[retval] = '\0';
        std::cout << CL_CLR << "Client: Reply with " << retval << " bytes received: " <<
                rec_buf.data() << std::endl;
    }
    else {
        int error = tcpip::get_last_error();
        std::cerr << "udp_client_oneshot: sendto failed with error: " <<
                 error << std::endl;
        WSACleanup();
        return 1;
    }
    return 0;
}
