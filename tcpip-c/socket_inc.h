#ifndef SOCKET_INC_H_
#define SOCKET_INC_H_

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#elif defined(__unix__)

#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#endif

#endif /* SOCKET_INC_H_ */
