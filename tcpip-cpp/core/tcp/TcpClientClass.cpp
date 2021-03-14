#include "TcpClientClass.h"

#include <utility.h>

#ifdef __unix__
#include <arpa/inet.h>
#endif

#include <iomanip>
#include <iostream>

TcpClientClass::TcpClientClass(tcpip::DemoConfig &cfg, size_t reception_buf_size):
        TcpipBase(cfg, reception_buf_size) {
}

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
            retval = tcp_read_operation();
        }
        else {
            return -1;
        }
        break;
    }
    case(dm::MD_2_OOP_CLIENT_NONE_SERVER_ONE): {
        if(step == Steps::SEND) {
            return shutdown(connect_socket, SHUT_SEND);
        }
        else if(step == Steps::READ) {
            return tcp_read_operation();
        }
        else {
           return -1;
        }
    }
    case(dm::MD_3_OOP_CLIENT_MUTLIPLE_SERVER_NO_REPLY): {
        if(step == Steps::SEND) {
            std::string string1 = "Hello, this is packet 1";
            std::string string2 = "Hello this is packet 2";
            std::string string3 = "Hello this is packet 3";
            std::queue<std::string> string_queue;
            string_queue.push(string1);
            string_queue.push(string2);
            string_queue.push(string3);
            return send_packets(string_queue);
        }
        else if(step == Steps::READ) {
            return tcp_read_operation();
        }
        else {
            return -1;
        }
    }
    case(dm::MD_4_OOP_CLIENT_MUTLIPLE_SERVER_MULTIPLE):
    default: {
        std::cout << "Client: Send operation not implemented yet" << std::endl;
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
        connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connect_socket < 0) {
            std::cerr << ANSI_COLOR_YELLOW << "TcpipBase::common_tcp_client_setup: "
                    "socketfailed with error: " << tcpip::get_last_error() << std::endl;
            return 1;
        }

        /* Connect to server. */
        retval = connect(connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (retval != 0) {
            connect_socket = INVALID_SOCKET;
            continue;
        }

        std::cout << "Client: Connected successfully to " << ip << std::endl;
        break;
    }

    freeaddrinfo(result);

    if (connect_socket < 0) {
        std::cout << "Unable to connect to server!" << std::endl;
        return 1;
    }
    return 0;
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
        std::cout  << CL_CLR << "Client: Sent " << send_buf.size() << " bytes string: " <<
                send_buf << std::endl;
    }

    retval = shutdown(connect_socket, SHUT_SEND);
    if(retval != 0) {
        std::cerr << "TcpClientClass::perform_send_operation: shutdown failed with error: " <<
                tcpip::get_last_error() << std::endl;
    }
    return retval;
}

int TcpClientClass::tcp_read_operation() {
    int retval = 0;
    /* Receive until the peer closes the connection */
    do {
        retval = recv(connect_socket, reinterpret_cast<char*>(reception_buffer.data()),
                reception_buffer.capacity() - 1, 0);
        if (retval > 0 ) {
            auto pg = print_guard();
            reception_buffer[retval] = '\0';
            uint8_t* ptr = reception_buffer.data();
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


int TcpClientClass::send_packets(std::queue<std::string>& string_queue) {
    int retval = 0;
    int idx = 0;
    while(not string_queue.empty()) {
        const auto string_ref = string_queue.front();
        std::cout << CL_CLR <<"Client: Sending string " << idx << ": " << string_ref.data() <<
                std::endl;
        int retval = send(connect_socket, reinterpret_cast<const char*>(string_ref.data()),
                string_ref.size(), 0);
        idx++;
        if (retval == SOCKET_ERROR) {
            std::cerr << "TcpClientClass::perform_send_operation: Send failed with error: " <<
                    tcpip::get_last_error() << std::endl;
            /* Clear the queue nonetheless */
            std::queue<std::string>().swap(string_queue);
            return 1;
        }
        string_queue.pop();
    }

    retval = shutdown(connect_socket, SHUT_SEND);
    if(retval != 0) {
        std::cerr << "TcpClientClass::perform_send_operation: shutdown failed with error: " <<
                tcpip::get_last_error() << std::endl;
    }
    return retval;
}

