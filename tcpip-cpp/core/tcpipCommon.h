#pragma once

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

using socket_t = SOCKET;

#elif defined(__unix__)

using socket_t = int;

#endif

namespace tcpip {

int getLastError();
void closeSocket(socket_t socket);

}

