#include "TcpServerClass.h"

#include <utility.h>

#ifdef __unix__
#include <arpa/inet.h>
#endif

#include <iostream>

TcpServerClass::TcpServerClass(tcpip::DemoConfig& cfg, size_t reception_buffer_size):
        TcpipBase(cfg, reception_buffer_size) {
}

TcpServerClass::~TcpServerClass() {
    tcpip::close_socket(listen_socket);
    tcpip::close_socket(client_socket);
}

int TcpServerClass::perform_operation() {
    if(type == tcpip::DemoTypes::CLIENT_ONLY) {
        return 0;
    }

    int retval = setup_server();
    if(retval != 0) {
        return retval;
    }

    retval = accept_connection();
    if(retval != 0) {
        return retval;
    }

    retval = perform_mode_operation();
    if(retval != 0) {
        return retval;
    }

    return perform_shutdown();
}

int TcpServerClass::perform_mode_operation() {
    using md = tcpip::DemoModes;
    switch(mode) {
    case(md::MD_0_PROCEDURAL_DEMO):
    case(md::MD_1_OOP_CLIENT_ONE_SERVER_ECHO): {
        return perform_mode_1_echo();
    }
    case(md::MD_2_OOP_CLIENT_NONE_SERVER_ONE): {
        return perform_mode_2();
    }
    case(md::MD_3_OOP_CLIENT_MUTLIPLE_SERVER_NO_REPLY): {
        return perform_mode_3();
    }
    case(md::MD_4_OOP_CLIENT_MUTLIPLE_SERVER_MULTIPLE):
    default: {
        std::cout << "TcpServerClass::perform_mode_operatio: Mode handling not implemented "
                "for mode" << static_cast<int>(mode) << "!" << std::endl;
    }
    }

    return 0;
}

int TcpServerClass::setup_server() {
    struct addrinfo hints = {};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    return setup(hints);
}

int TcpServerClass::setup(struct addrinfo &hints) {
    struct addrinfo *result = nullptr;

    /* Resolve the server address and port */
    int retval = 0;
    if(server_address == "any" or server_address == "") {
        /*
        See: https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
        and: https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo
         */
        hints.ai_flags = AI_PASSIVE;
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
                tcpip::get_last_error() << std::endl;
        freeaddrinfo(result);
        return 1;
    }

    freeaddrinfo(result);

    retval = listen(listen_socket, SOMAXCONN);
    if (retval != 0) {
        std::cerr << "TcpServerClass::setup_server: listen failed with error: " <<
                tcpip::get_last_error() << std::endl;
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

int TcpServerClass::perform_mode_1_echo() {
    int retval = 0;
    /* Receive until the peer shuts down the connection */
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
                std::cerr << "TCP Server: TcpServerClass::perform_simple_echo_op: "
                        "Send failed with error: " << tcpip::get_last_error() << std::endl;
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
            std::cerr << "Server: recv failed with error: " << tcpip::get_last_error() << std::endl;
            return 1;
        }

    } while (retval > 0);
    return 0;
}

int TcpServerClass::perform_mode_2() {
    int retval = 0;
    /* Receive until the peer shuts down the connection */
    do {
        int send_result;
        retval = recv(client_socket, reinterpret_cast<char*>(reception_buffer.data()),
                reception_buffer.capacity() - 1, 0);
        bool reply_sent = false;
        if(retval > 0) {
            std::cerr << "Server: Packet received from client "
                    "when none should have been sent!" << std::endl;
        }
        else if(retval == 0) {
            std::cout << SRV_CLR << "Server: No packets received from client" << std::endl;
            if(not reply_sent) {
                std::cout << SRV_CLR << "Server: Sending back unrequested reply" << std::endl;
                std::string reply = "Hello, this is my unrequested reply";
                send_result = send(client_socket, reinterpret_cast<char*>(reply.data()),
                        reply.size(), 0);
                if (send_result == SOCKET_ERROR) {
                    std::cerr << "Server: TcpServerClass::perform_simple_echo_op: "
                            "Send failed with error: " << tcpip::get_last_error() << std::endl;
                    return 1;
                }
                reply_sent = true;
            }
            else {
                std::cerr << "Server: More than 1 packet received from client!" << std::endl;
            }
        }
        else {
            std::cerr << "Server: recv failed with error: " << tcpip::get_last_error() << std::endl;
            return 1;
        }
    } while (retval > 0);
    return 0;
}

int TcpServerClass::perform_mode_3() {
    int retval = 0;
    int index = 0;
    /* Receive until the peer shuts down the connection */
    do {
        int send_result;
        retval = recv(client_socket, reinterpret_cast<char*>(reception_buffer.data()),
                reception_buffer.capacity() - 1, 0);
        bool reply_sent = false;
        if(retval > 0) {
            std::cout << SRV_CLR << "Server: Received packet " << index << " with " << retval <<
                    " bytes" << std::endl;
            index++;
        }
        else if(retval == 0) {
            std::cout << SRV_CLR << "Server: Client closed connection" << std::endl;
        }
        else {
            std::cerr << "Server: recv failed with error: " << tcpip::get_last_error() << std::endl;
            return 1;
        }
    } while (retval > 0);
    return 0;
}

int TcpServerClass::perform_shutdown() {
    {
        auto pg = print_guard();
        std::cout << SRV_CLR << "Server: Closing connection" << std::endl;
    }

    /* shutdown the connection since we're done */
    int retval = shutdown(client_socket, SHUT_SEND);
    if (retval == SOCKET_ERROR) {
        std::cerr << "shutdown failed with error: " << tcpip::get_last_error() << std::endl;
        return 1;
    }
    return 0;
}
