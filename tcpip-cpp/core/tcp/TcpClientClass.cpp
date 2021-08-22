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
    tcpip::closeSocket(connectSocket);
}

int TcpClientClass::performOperation() {
    if(type == tcpip::DemoTypes::SERVER_ONLY) {
        return 0;
    }

    int retval = commonConnectionAttempt();
    if(retval != 0) {
        return retval;
    }

    std::cout << CL_CLR << "Client: " << std::put_time(get_local_time(), "%H:%M:%S") << " | "
            << "Connection to server established" << std::endl;

    /* We are not connected and can perform send and receive operations */
    retval = performSendOperation();
    if(retval != 0) {
        return retval;
    }

    retval = performRecvOperation();
    if(retval != 0) {
        return retval;
    }
    return retval;
}

int TcpClientClass::performSendOperation() {
    return performOpCommon(Steps::SEND);
}

int TcpClientClass::performRecvOperation() {
    return performOpCommon(Steps::READ);
}

int TcpClientClass::performOpCommon(Steps step) {
    int retval = 0;
    using dm = tcpip::DemoModes;
    switch(mode) {
    case(dm::MD_0_PROCEDURAL_DEMO):
    case(dm::MD_1_OOP_CLIENT_ONE_SERVER_ECHO): {
        if(step == Steps::SEND) {
            return performSimpleSendOp();
        }
        else if(step == Steps::READ) {
            retval = tcpReadUntilServerFinishedOperation();
        }
        else {
            return -1;
        }
        break;
    }
    case(dm::MD_2_OOP_CLIENT_NONE_SERVER_ONE): {
        if(step == Steps::SEND) {
            return shutdown(connectSocket, SHUT_SEND);
        }
        else if(step == Steps::READ) {
            return tcpReadUntilServerFinishedOperation();
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
            return sendPackets(string_queue);
        }
        else if(step == Steps::READ) {
            return tcpReadUntilServerFinishedOperation();
        }
        else {
            return -1;
        }
    }
    case(dm::MD_4_OOP_CLIENT_MUTLIPLE_SERVER_MULTIPLE): {
      break;
    }
    case(dm::MD_5_USE_SELECT): {
      if(step == Steps::SEND) {
          return performSimpleSendOp();
      }
      else if(step == Steps::READ) {
        int retval = 0;
        // Receive packets and then close connection
        do {
            retval = recv(connectSocket, reinterpret_cast<char*>(reception_buffer.data()),
                    reception_buffer.capacity() - 1, 0);
            if (retval > 0 ) {
                auto pg = print_guard();
                reception_buffer[retval] = '\0';
                uint8_t* ptr = reception_buffer.data();
                std::cout  << CL_CLR << "Client: Received " << retval << " bytes string: " <<
                    reception_buffer.data() << std::endl;
            }
        } while(retval > 0 );
        // close connection
        tcpip::closeSocket(connectSocket);
      }
      break;
    }
    default: {
        std::cout << "Client: Send operation not implemented yet" << std::endl;
    }
    }
    return retval;
}

int TcpClientClass::commonConnectionAttempt() {
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
        connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (connectSocket < 0) {
            std::cerr << ANSI_COLOR_YELLOW << "TcpipBase::common_tcp_client_setup: "
                    "socketfailed with error: " << tcpip::getLastError() << std::endl;
            return 1;
        }

        /* Connect to server. */
        retval = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (retval != 0) {
            connectSocket = INVALID_SOCKET;
            continue;
        }

        std::cout << "Client: Connected successfully to " << ip << std::endl;
        break;
    }

    freeaddrinfo(result);

    if (connectSocket < 0) {
        std::cout << "Unable to connect to server!" << std::endl;
        return 1;
    }
    return 0;
}

int TcpClientClass::performSimpleSendOp() {
    std::string send_buf = "this is a test";
    int retval = send(connectSocket, send_buf.c_str(), send_buf.size(), 0 );
    if (retval == SOCKET_ERROR) {
        std::cerr << "TcpClientClass::perform_send_operation: Send failed with error: " <<
                tcpip::getLastError() << std::endl;
        return 1;
    }

    {
        auto pg = print_guard();
        std::cout  << CL_CLR << "Client: Sent " << send_buf.size() << " bytes string: " <<
                send_buf << std::endl;
    }

    retval = shutdown(connectSocket, SHUT_SEND);
    if(retval != 0) {
        std::cerr << "TcpClientClass::perform_send_operation: shutdown failed with error: " <<
                tcpip::getLastError() << std::endl;
    }
    return retval;
}

int TcpClientClass::tcpReadUntilServerFinishedOperation() {
    int retval = 0;
    /* Receive until the peer closes the connection */
    do {
        retval = recv(connectSocket, reinterpret_cast<char*>(reception_buffer.data()),
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
                    tcpip::getLastError() << std::endl;
        }
    } while(retval > 0 );
    return 0;
}


int TcpClientClass::sendPackets(std::queue<std::string>& string_queue) {
    int retval = 0;
    int idx = 0;
    while(not string_queue.empty()) {
        const auto string_ref = string_queue.front();
        std::cout << CL_CLR <<"Client: Sending string " << idx << ": " << string_ref.data() <<
                std::endl;
        int retval = send(connectSocket, reinterpret_cast<const char*>(string_ref.data()),
                string_ref.size(), 0);
        idx++;
        if (retval == SOCKET_ERROR) {
            std::cerr << "TcpClientClass::perform_send_operation: Send failed with error: " <<
                    tcpip::getLastError() << std::endl;
            /* Clear the queue nonetheless */
            std::queue<std::string>().swap(string_queue);
            return 1;
        }
        string_queue.pop();
    }

    retval = shutdown(connectSocket, SHUT_SEND);
    if(retval != 0) {
        std::cerr << "TcpClientClass::perform_send_operation: shutdown failed with error: " <<
                tcpip::getLastError() << std::endl;
    }
    return retval;
}

