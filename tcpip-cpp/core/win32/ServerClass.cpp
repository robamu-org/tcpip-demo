#include "../ServerClass.h"

#include <utility.h>

#include <iostream>
#include <iomanip>
#include <stdexcept>

ServerClass::ServerClass(tcpip::DemoConfig& cfg, size_t reception_buf_size):
        TcpipBase(cfg, reception_buf_size) {
}

ServerClass::~ServerClass() {
    closesocket(listen_socket);
    closesocket(client_socket);
}

int ServerClass::perform_operation() {
    int retval = setup_server();
    if(retval != 0) {
        return retval;
    }

    retval = accept_connection();
    if(retval != 0) {
        return retval;
    }

    retval = perform_mode_operation();
    return retval;
}


int ServerClass::setup_server() {
    struct addrinfo hints = {};
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    return setup(hints);
}

int ServerClass::setup(struct addrinfo &hints) {
    struct addrinfo *result = NULL;
    int retval = 0;

    // Resolve the server address and port
    retval = getaddrinfo(nullptr, tcpip::SERVER_PORT, &hints, &result);
    if (retval != 0) {
        std::cerr << "ServerClass::setup_server: getaddrinfo failed with error: " <<
                retval << std::endl;
        return 1;
    }

    // Create a SOCKET for connecting to server
    listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        std::cerr << "ServerClass::setup_server: socket failed with error: " <<
                WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        return 1;
    }

    // Setup the TCP listening socket
    retval = bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
    if (retval == SOCKET_ERROR) {
        std::cerr << "ServerClass::setup_server: bind failed with error: " <<
                WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        return 1;
    }

    freeaddrinfo(result);

    retval = listen(listen_socket, SOMAXCONN);
    if (retval == SOCKET_ERROR) {
        std::cerr << "ServerClass::setup_server: listen failed with error: " <<
                WSAGetLastError() << std::endl;
        return 1;
    }
    return 0;
}

int ServerClass::accept_connection() {
    // Accept a client socket
    client_socket = accept(listen_socket, NULL, NULL);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "ServerClass::setup_server: accept failed with error: " <<
                WSAGetLastError() << std::endl;
        return 1;
    }

    // No longer need server socket
    closesocket(listen_socket);
    return 0;
}

int ServerClass::perform_mode_operation() {
    using md = tcpip::DemoModes;
    switch(mode) {
    case(md::MD_1_OOP_CLIENT_ONE_SERVER_ECHO): {
        return perform_simple_echo_op();
    }
    case(md::MD_2_OOP_CLIENT_NONE_SERVER_ONE):
    case(md::MD_3_OOP_CLIENT_MUTLIPLE_SERVER_NO_REPLY):
    case(md::MD_4_OOP_CLIENT_MUTLIPLE_SERVER_MULTIPLE):
    default: {
        std::cout << "ServerClass::perform_mode_operatio: Mode handling not implemented for mode" <<
                static_cast<int>(mode) << "!" << std::endl;
    }
    }

    return 0;
}

int ServerClass::perform_simple_echo_op() {
    int retval = 0;
    // Receive until the peer shuts down the connection
    do {
        int send_result;

        retval = recv(client_socket, reinterpret_cast<char*>(reception_buffer.data()),
                reception_buffer.capacity() - 1, 0);
        if (retval > 0) {
            {
                auto pg = print_guard();
                reception_buffer[retval] = '\0';
                std::cout << SRV_CLR << "Server: Received " << retval << " bytes string: "
                        << reception_buffer.data() << std::endl;
            }

            size_t bytes_to_sendback = retval;
            // Echo the buffer back to the sender
            send_result = send(client_socket, reinterpret_cast<char*>(reception_buffer.data()),
                    bytes_to_sendback, 0);
            if (send_result == SOCKET_ERROR) {
                std::cerr << "send failed with error: " << WSAGetLastError() << std::endl;
                return 1;
            }

            {
                auto pg = print_guard();
                std::cout << SRV_CLR << "Server: Bytes echoed back: " << send_result << std::endl;
            }
        }
        else if (retval == 0) {
            std::cout << SRV_CLR << "Server: Client closed connection" << std::endl;
        }
        else  {
            std::cerr << "Server: recv failed with error: " << WSAGetLastError() << std::endl;
            return 1;
        }

    } while (retval > 0);

    {
        auto pg = print_guard();
        std::cout << "Server: Closing connection" << std::endl;
    }

    // shutdown the connection since we're done
    retval = shutdown(client_socket, SD_SEND);
    if (retval == SOCKET_ERROR) {
        std::cerr << "shutdown failed with error: " << WSAGetLastError() << std::endl;
        return 1;
    }
    return 0;
}
