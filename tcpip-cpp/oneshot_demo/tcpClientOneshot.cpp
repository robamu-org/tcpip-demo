#include "demoConfig.h"
#include "spdlog/spdlog.h"

#include <iostream>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#elif defined(__unix__)

#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#endif

#include <vector>
#include <cstring>


#ifdef _WIN32
int tcpClientOneshot(std::string server_address) {
  // Totally not copied from https://docs.microsoft.com/en-us/windows/win32/winsock/complete-client-code
  WSADATA wsaData;
  SOCKET ConnectSocket = INVALID_SOCKET;
  struct addrinfo *result = NULL, *ptr = NULL, hints;
  const char *sendbuf = "this is a test";
  std::vector<uint8_t> reception_buffer(tcpip::BUFFER_SIZES);
  int iResult;
  int recvbuflen = tcpip::BUFFER_SIZES;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != 0) {
    printf("WSAStartup failed with error: %d\n", iResult);
    return 1;
  }

  ZeroMemory( &hints, sizeof(hints) );
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // Resolve the server address and port
  iResult = getaddrinfo(server_address.c_str(), tcpip::SERVER_PORT, &hints, &result);
  if ( iResult != 0 ) {
    printf("getaddrinfo failed with error: %d\n", iResult);
    WSACleanup();
    return 1;
  }

  // Attempt to connect to an address until one succeeds
  for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
    struct sockaddr_in *addr_in = (struct sockaddr_in *)ptr->ai_addr;
    char *ip = inet_ntoa(addr_in->sin_addr);
    spdlog::info("{}: Attempting connection to address {}", tcpip::CLIENT_PR, ip);
    // Create a SOCKET for connecting to server
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
      printf("socket failed with error: %d\n", WSAGetLastError());
      WSACleanup();
      return 1;
    }

    // Connect to server.
    iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
      closesocket(ConnectSocket);
      ConnectSocket = INVALID_SOCKET;
      continue;
    }


    spdlog::info("{}: Connect success", tcpip::CLIENT_PR);
    break;
  }

  freeaddrinfo(result);

  if (ConnectSocket == INVALID_SOCKET) {
    printf("Unable to connect to server!\n");
    WSACleanup();
    return 1;
  }

  // Send an initial buffer
  iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
  if (iResult == SOCKET_ERROR) {
    printf("send failed with error: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
    return 1;
  }

  spdlog::info("{}: Sent {} bytes: {}", tcpip::CLIENT_PR, iResult, sendbuf);

  // shutdown the connection since no more data will be sent
  iResult = shutdown(ConnectSocket, SD_SEND);
  if (iResult == SOCKET_ERROR) {
    printf("shutdown failed with error: %d\n", WSAGetLastError());
    closesocket(ConnectSocket);
    WSACleanup();
    return 1;
  }

  // Receive until the peer closes the connection
  do {

    iResult = recv(ConnectSocket, reinterpret_cast<char*>(reception_buffer.data()), recvbuflen, 0);
    if ( iResult > 0 ) {
      spdlog::info("{}: Received {} bytes: {}", tcpip::CLIENT_PR, iResult, reception_buffer.data());
    }
    else if ( iResult == 0 )
      spdlog::info("{}: Server closed connection", tcpip::CLIENT_PR);
    else
      printf(CL_CLR "Client: recv failed with error: %d\n", WSAGetLastError());

  } while( iResult > 0 );

  // cleanup
  closesocket(ConnectSocket);
  WSACleanup();

  return 0;
}

#elif defined(__unix__)

int tcpClientOneshot(std::string server_address) {
  /* Based on https://docs.microsoft.com/en-us/windows/win32/winsock/complete-client-code */
  int connect_socket = 0;
  struct addrinfo *result = NULL;
  struct addrinfo hints = {};

  const char *sendbuf = "this is a test";
  std::vector<uint8_t> reception_buffer(tcpip::BUFFER_SIZES);
  int retval;
  int recvbuflen = tcpip::BUFFER_SIZES;

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // Resolve the server address and port
  retval = getaddrinfo(server_address.c_str(), tcpip::SERVER_PORT, &hints, &result);
  if (retval != 0) {
    printf("getaddrinfo failed with error: %d\n", retval);
    return 1;
  }

  // Attempt to connect to an address until one succeeds
  for(struct addrinfo* ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
    struct sockaddr_in *addr_in = (struct sockaddr_in *)ptr->ai_addr;
    char *ip = inet_ntoa(addr_in->sin_addr);
    spdlog::info("{}: Attemtping connection to address {}", tcpip::CLIENT_PR, ip);
    // Create a SOCKET for connecting to server
    connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (connect_socket == 0) {
      printf("Client: socket failed with error: %d\n", errno);
      return 1;
    }

    // Connect to server.
    retval = connect(connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (retval != 0) {
      close(connect_socket);
      connect_socket = -1;
      continue;
    }

    spdlog::info("{}: Connect success", tcpip::CLIENT_PR);
    break;
  }

  freeaddrinfo(result);

  if (connect_socket < 0) {
    printf("Client: Unable to connect to server!\n");
    return 1;
  }

  spdlog::info("{}: Sending string {}", tcpip::CLIENT_PR, sendbuf);
  // Send an initial buffer
  retval = send(connect_socket, sendbuf, (int)strlen(sendbuf), 0 );
  if (retval < 0) {
    printf("send failed with error: %d\n", errno);
    close(connect_socket);
    return 1;
  }

  spdlog::info("{}: Bytes sent: {}", tcpip::CLIENT_PR, retval);

  // shutdown the connection since no more data will be sent
  retval = shutdown(connect_socket, SHUT_WR);
  if (retval != 0) {
    printf("shutdown failed with error: %d\n", errno);
    close(connect_socket);
    return 1;
  }

  // Receive until the peer closes the connection
  do {

    retval = recv(connect_socket, reinterpret_cast<char*>(reception_buffer.data()), recvbuflen, 0);
    if (retval > 0) {
      spdlog::info("{}: Bytes received: {}: {}", tcpip::CLIENT_PR, retval, reception_buffer.data());
    }
    else if (retval == 0) {
      spdlog::info("{}: Connection closed", tcpip::CLIENT_PR);
    }
    else {
      printf(ANSI_COLOR_CYAN "Client: recv failed with error: %d\n", errno);
    }

  } while(retval > 0);

  // cleanup
  close(connect_socket);

  return 0;
}


#endif /* UNIX */
