#pragma once

#ifdef _WIN32

#include <winsock2.h>
using socket_t = SOCKET;

#elif defined(__unix__)

using socket_t = int;

#endif

namespace tcpip {

int get_last_error();
void close_socket(socket_t socket);

}

