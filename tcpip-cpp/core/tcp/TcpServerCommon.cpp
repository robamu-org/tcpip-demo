#include "TcpServerClass.h"

#ifdef __unix__
#include <arpa/inet.h>
#endif

#include <iostream>

int TcpServerClass::common_tcp_server_setup(struct addrinfo& hints) {
    struct addrinfo *result = nullptr;

    /* Resolve the server address and port */
    int retval = 0;
    if(server_address == "any" or server_address == "") {
        retval = getaddrinfo(nullptr, server_port.c_str(), &hints, &result);
    }
    else {
        retval = getaddrinfo(server_address.c_str(), server_port.c_str(), &hints, &result);
    }

    if (retval != 0) {
        std::cerr << "TcpServerClass::setup_server: getaddrinfo failed with error: " <<
                retval << std::endl;
        return 1;
    }

    /* Create a socket for connecting to server */
    listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listen_socket < 0) {
        std::cerr << "TcpServerClass::setup_server: socket failed with error: " <<
                errno << std::endl;
        freeaddrinfo(result);
        return 1;
    }

    /* Setup the TCP listening socket */
    retval = bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
    if (retval != 0) {
        std::cerr << "TcpServerClass::setup_server: bind failed with error: " <<
                errno << std::endl;
        freeaddrinfo(result);
        return 1;
    }

    freeaddrinfo(result);

    retval = listen(listen_socket, SOMAXCONN);
    if (retval != 0) {
        std::cerr << "TcpServerClass::setup_server: listen failed with error: " <<
                errno << std::endl;
        return 1;
    }
    return 0;
}

int TcpServerClass::accept_connection() {
    /* Accept a client socket */
    client_socket = accept(listen_socket, NULL, NULL);
    if (client_socket < 0) {
        std::cerr << "TcpServerClass::setup_server: accept failed with error: " <<
                tcpip::get_last_error() << std::endl;
        return 1;
    }

    /* No longer need server socket */
    tcpip::close_socket(listen_socket);
    return 0;
}
