#include "TcpClientClass.h"

#include <utility.h>

#ifdef __unix__
#include <arpa/inet.h>
#endif

#include <iomanip>
#include <iostream>


TcpClientClass::~TcpClientClass() {
    tcpip::close_socket(connect_socket);
}

int TcpClientClass::perform_operation() {
    if(type == tcpip::DemoTypes::SERVER_ONLY) {
        return 0;
    }

    int retval = common_connection_attempt();
    if(retval != 0) {
        return retval;
    }

    std::cout << CL_CLR << "Client: " << std::put_time(get_local_time(), "%H:%M:%S") << " | "
            << "Connection to server established" << std::endl;

    /* We are not connected and can perform send and receive operations */
    retval = perform_send_operation();
    if(retval != 0) {
        return retval;
    }

    retval = perform_recv_operation();
    if(retval != 0) {
        return retval;
    }
    return retval;
}

int TcpClientClass::perform_send_operation() {
    return perform_op_common(Steps::SEND);
}

int TcpClientClass::perform_recv_operation() {
    return perform_op_common(Steps::READ);
}

int TcpClientClass::perform_op_common(Steps step) {
    int retval = 0;
    using dm = tcpip::DemoModes;
    switch(mode) {
    case(dm::MD_0_PROCEDURAL_DEMO):
    case(dm::MD_1_OOP_CLIENT_ONE_SERVER_ECHO): {
        if(step == Steps::SEND) {
            return perform_simple_send_op();
        }
        else if(step == Steps::READ) {
            retval = perform_echo_recv_operation();
        }
        break;
    }
    case(dm::MD_2_OOP_CLIENT_NONE_SERVER_ONE):
    case(dm::MD_3_OOP_CLIENT_MUTLIPLE_SERVER_NO_REPLY):
    case(dm::MD_4_OOP_CLIENT_MUTLIPLE_SERVER_MULTIPLE):
    default: {
        std::cout << "Send operation not implmented yet" << std::endl;
    }
    }
    return retval;
}

int TcpClientClass::common_connection_attempt() {
    struct addrinfo hints = {};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    return setup(hints);
}

int TcpClientClass::setup(struct addrinfo& hints) {
    return common_tcp_client_setup(hints, connect_socket);
}

int TcpClientClass::perform_simple_send_op() {
    std::string send_buf = "this is a test";
    int retval = send(connect_socket, send_buf.c_str(), send_buf.size(), 0 );
    if (retval == SOCKET_ERROR) {
        std::cerr << "TcpClientClass::perform_send_operation: Send failed with error: " <<
                tcpip::get_last_error() << std::endl;
        return 1;
    }

    {
        auto pg = print_guard();
        std::cout  << CL_CLR << "Client: " << std::put_time(get_local_time(), "%T") <<
                "Sent " << send_buf.size() << " bytes string: " << send_buf << std::endl;
    }

    retval = shutdown(connect_socket, SHUT_SEND);
    if(retval != 0) {
        std::cerr << "TcpClientClass::perform_send_operation: shutdown failed with error: " <<
                tcpip::get_last_error() << std::endl;
    }
    return 0;
}


int TcpClientClass::common_tcp_client_setup(struct addrinfo& hints, socket_t& conn_sock) {
    struct addrinfo* result = nullptr;
    int retval = 0;

    if(server_address == "" or server_address == "nullptr") {
        int retval = getaddrinfo(nullptr, server_port.c_str(), &hints, &result);
    }
    else {
        int retval = getaddrinfo(server_address.c_str(), server_port.c_str(), &hints, &result);
    }
    if(retval != 0) {
        std::cerr << "getaddrinfo failed with error: " << retval << std::endl;
    }

    for(auto ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *)ptr->ai_addr;
        char *ip = inet_ntoa(addr_in->sin_addr);
        std::cout << "Client: Attempting connection to address " << ip << std::endl;
        /* Create a socket for connecting to server */
        conn_sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (conn_sock < 0) {
#ifdef _WIN32
            int error =  WSAGetLastError();
#elif defined(__unix__)
            int error = errno;
#endif
            std::cerr << ANSI_COLOR_YELLOW << "TcpipBase::common_tcp_client_setup: "
                    "socketfailed with error: " << error << std::endl;
            return 1;
        }

        /* Connect to server. */
        retval = connect(conn_sock, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (retval != 0) {
            conn_sock = INVALID_SOCKET;
            continue;
        }

        std::cout << "Client: Connected successfully to " << ip << std::endl;
        break;
    }

    freeaddrinfo(result);

    if (conn_sock < 0) {
        std::cout << "Unable to connect to server!" << std::endl;
        return 1;
    }
    return 0;
}



int TcpClientClass::perform_echo_recv_operation() {
    int retval = 0;
    /* Receive until the peer closes the connection */
    do {
        retval = recv(connect_socket, reinterpret_cast<char*>(reception_buffer.data()),
                reception_buffer.capacity() - 1, 0);
        if (retval > 0 ) {
            auto pg = print_guard();
            reception_buffer[retval] = '\0';
            std::cout  << CL_CLR << "Client: Received " << retval << " bytes string: " <<
                    reception_buffer.data() << std::endl;
        }
        else if (retval == 0 ) {
            auto pg = print_guard();
            std::cout << CL_CLR << "Client: Server closed connection" << std::endl;
        }

        else {
            std::cerr << ANSI_COLOR_RED << "Client: recv failed with error " <<
                    tcpip::get_last_error() << std::endl;
        }
    } while(retval > 0 );
    return 0;
}
