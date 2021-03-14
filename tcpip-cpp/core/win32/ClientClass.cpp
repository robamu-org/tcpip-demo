#include "../ClientClass.h"
#include <utility.h>

#include <iostream>
#include <iomanip>


ClientClass::ClientClass(tcpip::DemoConfig& cfg, size_t reception_buf_size):
        TcpipBase(cfg, reception_buf_size) {
    WSADATA wsaData;
    // Initialize Winsock
    int retval = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (retval != 0) {
        std::cerr << "WSAStartup failed with error: " << retval << std::endl;
        throw std::runtime_error("WSAStartup failed!");
    }
}

ClientClass::~ClientClass() {
    closesocket(connect_socket);
    WSACleanup();
}

int ClientClass::perform_operation() {
    if(type == tcpip::DemoTypes::SERVER_ONLY) {
        return 0;
    }

    int retval = attempt_connection();
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

int ClientClass::attempt_connection() {
    int retval = 0;

    struct addrinfo *result = nullptr;
    struct addrinfo hints = {};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

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
        // Create a SOCKET for connecting to server
        connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connect_socket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            return 1;
        }

        // Connect to server.
        retval = connect(connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (retval == SOCKET_ERROR) {
            connect_socket = INVALID_SOCKET;
            continue;
        }

        std::cout << "Client: Connected successfully to " << ip << std::endl;
        break;
    }

    freeaddrinfo(result);

    if (connect_socket == INVALID_SOCKET) {
        std::cout << "Unable to connect to server!" << std::endl;
        return 1;
    }
    return 0;
}

int ClientClass::perform_send_operation() {
    int retval = 0;
    using dm = tcpip::DemoModes;
    switch(mode) {
    case(dm::MD_1_OOP_CLIENT_ONE_SERVER_ECHO): {
        retval = perform_simple_send_op();
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

int ClientClass::perform_recv_operation() {
    int retval = 0;
    using dm = tcpip::DemoModes;
    switch(mode) {
    case(dm::MD_1_OOP_CLIENT_ONE_SERVER_ECHO): {
        perform_echo_recv_operation();
        break;
    }
    case(dm::MD_2_OOP_CLIENT_NONE_SERVER_ONE):
    case(dm::MD_3_OOP_CLIENT_MUTLIPLE_SERVER_NO_REPLY):
    case(dm::MD_4_OOP_CLIENT_MUTLIPLE_SERVER_MULTIPLE):
    default: {
        std::cout << "Send operation not implmented yet" << std::endl;
    }
    }
    return 0;
}

int ClientClass::perform_simple_send_op() {
    std::string send_buf = "this is a test";
    int retval = send(connect_socket, send_buf.c_str(), send_buf.size(), 0 );
    if (retval == SOCKET_ERROR) {
        std::cerr << "ClientClass::perform_send_operation: Send failed with error: " <<
                WSAGetLastError() << std::endl;
        return 1;
    }

    {
        auto pg = print_guard();
        std::cout  << CL_CLR << "Client: " <<std::put_time(get_local_time(), "%T") <<
                "Sent " << send_buf.size() << " bytes string: " << send_buf << std::endl;
    }

    retval = shutdown(connect_socket, SD_SEND);
    if(retval != 0) {
        std::cerr << "ClientClass::perform_send_operation: shutdown failed with error: " <<
                WSAGetLastError() << std::endl;
    }
    return 0;
}

int ClientClass::perform_echo_recv_operation() {
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
            std::cerr << ANSI_COLOR_RED << "Client: recv failed with error " << WSAGetLastError()
                    << std::endl;
        }
    } while(retval > 0 );
    return 0;
}
