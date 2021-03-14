#include "../TcpServerClass.h"

#include <unistd.h>
#include <netdb.h>

#include <iostream>
#include <stdexcept>

TcpServerClass::TcpServerClass(tcpip::DemoConfig& cfg, size_t reception_buffer_size):
TcpipBase(cfg, reception_buffer_size) {
}

TcpServerClass::~TcpServerClass() {
    close(listen_socket);
    close(client_socket);
}

int TcpServerClass::perform_operation() {
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

int TcpServerClass::setup_server() {
    struct addrinfo hints = {};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    return setup(hints);
}

int TcpServerClass::setup(struct addrinfo &hints) {
    return common_tcp_server_setup(hints);
}

int TcpServerClass::accept_connection() {
    /* Accept a client socket */
    client_socket = accept(listen_socket, NULL, NULL);
    if (client_socket < 0) {
        std::cerr << "TcpServerClass::setup_server: accept failed with error: " <<
                errno << std::endl;
        return 1;
    }

    /* No longer need server socket */
    close(listen_socket);
    return 0;
}

int TcpServerClass::perform_mode_operation() {
    using md = tcpip::DemoModes;
    switch(mode) {
    case(md::MD_1_OOP_CLIENT_ONE_SERVER_ECHO): {
        return perform_simple_echo_op();
    }
    case(md::MD_3_OOP_CLIENT_MUTLIPLE_SERVER_NO_REPLY):
    case(md::MD_4_OOP_CLIENT_MUTLIPLE_SERVER_MULTIPLE):
    default: {
        std::cout << "TcpServerClass::perform_mode_operatio: Mode handling not implemented for mode" <<
                static_cast<int>(mode) << "!" << std::endl;
    }
    }

    return 0;
}

int TcpServerClass::perform_simple_echo_op() {
    int retval = 0;
    // Receive until the peer shuts down the connection
    do {
        int send_result;

        retval = recv(client_socket, reinterpret_cast<char*>(reception_buffer.data()),
                reception_buffer.capacity(), 0);
        if (retval > 0) {
            std::cout << "Server: Bytes Received: " << retval << std::endl;
            size_t bytes_to_sendback = retval;
            // Echo the buffer back to the sender
            send_result = send(client_socket, reinterpret_cast<char*>(reception_buffer.data()),
                    bytes_to_sendback, 0);
            if (send_result < 0) {
                std::cerr << "send failed with error: " << errno << std::endl;
                return 1;
            }
            std::cout << "Server: Bytes sent: " << send_result << std::endl;
        }
        else if (retval == 0)
            printf("Server: Connection closing...\n");
        else  {
            std::cerr << "Server: recv failed with error: " << errno << std::endl;
            return 1;
        }

    } while (retval > 0);

    // shutdown the connection since we're done
    retval = shutdown(client_socket, SHUT_WR);
    if (retval != 0) {
        std::cerr << "shutdown failed with error: " << errno << std::endl;
        return 1;
    }
    return 0;
}
