#include "TcpServerClass.h"
#include "spdlog/spdlog.h"

#include <thread>
#include <utility.h>

#ifdef __unix__
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <iostream>

TcpServerClass::TcpServerClass(tcpip::DemoConfig& cfg, size_t reception_buffer_size):
TcpipBase(cfg, reception_buffer_size) {
}

TcpServerClass::~TcpServerClass() {
  tcpip::closeSocket(listenSocket);
  tcpip::closeSocket(clientSocket);
}

int TcpServerClass::performOperation() {
  if(type == tcpip::DemoTypes::CLIENT_ONLY) {
    return 0;
  }

  int retval = setupServer();
  if(retval != 0) {
    return retval;
  }

  retval = acceptConnection();
  if(retval != 0) {
    return retval;
  }

  retval = performModeOperation();
  if(retval != 0) {
    return retval;
  }

  return performShutdown();
}

int TcpServerClass::performModeOperation() {
  using md = tcpip::DemoModes;
  switch(mode) {
  case(md::MD_0_PROCEDURAL_DEMO):
  case(md::MD_1_OOP_CLIENT_ONE_SERVER_ECHO): {
    return perforMode1Echo();
  }
  case(md::MD_2_OOP_CLIENT_NONE_SERVER_ONE): {
    return performMode2();
  }
  case(md::MD_3_OOP_CLIENT_MUTLIPLE_SERVER_NO_REPLY): {
    return performMode3();
  }
  case(md::MD_5_USE_SELECT): {
    return performMode5();
  }
  case(md::MD_4_OOP_CLIENT_MUTLIPLE_SERVER_MULTIPLE):
  default: {
    std::cout << "TcpServerClass::performModeOperation: Mode handling not implemented "
        "for mode" << static_cast<int>(mode) << "!" << std::endl;
  }
  }

  return 0;
}

int TcpServerClass::setupServer() {
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
  listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (listenSocket < 0) {
    std::cerr << "TcpServerClass::setup_server: socket failed with error: " <<
        errno << std::endl;
    freeaddrinfo(result);
    return 1;
  }

  /* Setup the TCP listening socket */
  retval = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
  if (retval != 0) {
    std::cerr << "TcpServerClass::setup_server: bind failed with error: " <<
        tcpip::getLastError() << std::endl;
    freeaddrinfo(result);
    return 1;
  }

  freeaddrinfo(result);

  retval = listen(listenSocket, SOMAXCONN);
  if (retval != 0) {
    std::cerr << "TcpServerClass::setup_server: listen failed with error: " <<
        tcpip::getLastError() << std::endl;
    return 1;
  }
  return 0;
}


int TcpServerClass::acceptConnection() {
  /* Accept a client socket */
  clientSocket = accept(listenSocket, NULL, NULL);
  if (clientSocket < 0) {
    std::cerr << "TcpServerClass::setup_server: accept failed with error: " <<
        tcpip::getLastError() << std::endl;
    return 1;
  }

  /* No longer need server socket */
  tcpip::closeSocket(listenSocket);
  return 0;
}

int TcpServerClass::perforMode1Echo() {
  int retval = 0;
  /* Receive until the peer shuts down the connection */
  do {
    int send_result;

    retval = recv(clientSocket, reinterpret_cast<char*>(reception_buffer.data()),
        reception_buffer.capacity() - 1, 0);
    if (retval > 0) {
      {
        reception_buffer[retval] = '\0';
        spdlog::info("{}: Received packet with {} bytes", tcpip::SERVER_PR, retval);
      }

      size_t bytes_to_sendback = retval;
      // Echo the buffer back to the sender
      send_result = send(clientSocket, reinterpret_cast<char*>(reception_buffer.data()),
          bytes_to_sendback, 0);
      if (send_result == SOCKET_ERROR) {
        std::cerr << "TCP Server: TcpServerClass::perform_simple_echo_op: "
            "Send failed with error: " << tcpip::getLastError() << std::endl;
        return 1;
      }
      spdlog::info("{}: Bytes echoed back: {}", tcpip::SERVER_PR, send_result);
    }
    else if (retval == 0) {
      spdlog::info("{}: Client closed connection", tcpip::SERVER_PR);
    }
    else  {
      spdlog::error("{}: recv failed with error: {}", tcpip::SERVER_PR, tcpip::getLastError());
      return 1;
    }

  } while (retval > 0);
  return 0;
}

int TcpServerClass::performMode2() {
  int retval = 0;
  /* Receive until the peer shuts down the connection */
  do {
    int send_result;
    retval = recv(clientSocket, reinterpret_cast<char*>(reception_buffer.data()),
        reception_buffer.capacity() - 1, 0);
    bool reply_sent = false;
    if(retval > 0) {
      std::cerr << "Server: Packet received from client "
          "when none should have been sent!" << std::endl;
    }
    else if(retval == 0) {
      spdlog::info("{}: No packets received from client", tcpip::SERVER_PR);
      if(not reply_sent) {
        spdlog::info("{}: Sending back unrequested reply", tcpip::SERVER_PR);
        std::string reply = "Hello, this is my unrequested reply";
        send_result = send(clientSocket, reinterpret_cast<char*>(reply.data()),
            reply.size(), 0);
        if (send_result == SOCKET_ERROR) {
          std::cerr << "Server: TcpServerClass::perform_simple_echo_op: "
              "Send failed with error: " << tcpip::getLastError() << std::endl;
          return 1;
        }
        reply_sent = true;
      }
      else {
        spdlog::error("{}: More than 1 packet received from client!", tcpip::SERVER_PR);
      }
    }
    else {
      std::cerr << "Server: recv failed with error: " << tcpip::getLastError() << std::endl;
      return 1;
    }
  } while (retval > 0);
  return 0;
}

int TcpServerClass::performMode3() {
  int retval = 0;
  uint32_t index = 0;
  /* Receive until the peer shuts down the connection */
  do {
    int send_result;
    receiveData(&index);
  } while (retval > 0);
  return 0;
}

int TcpServerClass::performShutdown() {
  spdlog::info("{}: Closing connection", tcpip::SERVER_PR);

  /* shutdown the connection since we're done */
  int retval = shutdown(clientSocket, SHUT_SEND);
  if (retval == SOCKET_ERROR) {
    std::cerr << "shutdown failed with error: " << tcpip::getLastError() << std::endl;
    return 1;
  }
  return 0;
}

int TcpServerClass::performMode5() {
  using namespace std::chrono_literals;
  // Receive until the peer shuts down the connection, use select to do this
  fd_set rfds;
  fd_set efds;

  FD_ZERO(&rfds);
  FD_SET(clientSocket, &rfds);

  FD_ZERO(&efds);
  FD_SET(clientSocket, &efds);

  timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;

  // Should be set to highest-numbered file descriptor plus 1
  int nfds = 1 + 1;

  while (true) {
    uint32_t index = 0;
    int retval = select(clientSocket + 1, &rfds, nullptr, &efds, &tv);
    if(retval < 0) {
      // client might have shut down connection?
      std::cerr << "select: Error " << tcpip::getLastError() << std::endl;
    }
    else if(retval > 0) {
      if(FD_ISSET(clientSocket, &rfds)) {
        // data available
        int result = receiveData();
        if(result == 0) {
          break;
        }
      }
      if(FD_ISSET(clientSocket, &efds)) {
        std::cout << "Client closed?" << std::endl;
      }
    }
    else {
      // no data available
      std::this_thread::sleep_for(500ms);
    }
  }
  return 0;
}

int TcpServerClass::receiveData(uint32_t* index) {
  int retval = recv(clientSocket, reinterpret_cast<char*>(reception_buffer.data()),
      reception_buffer.capacity() - 1, 0);
  bool reply_sent = false;
  if(retval > 0) {
    spdlog::info("{}: Received packet with {} bytes", tcpip::SERVER_PR, retval);
    if(index != nullptr) {
      (*index)++;
    }
    return 1;
  }
  else if(retval == 0) {
    std::cout << SRV_CLR << "Server: Client closed connection" << std::endl;
    return 0;
  }
  else {
    std::cerr << "Server: recv failed with error: " << tcpip::getLastError() << std::endl;
    return -1;
  }
}
