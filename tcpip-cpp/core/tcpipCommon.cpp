#include "tcpipCommon.h"

#ifdef _WIN32

#elif defined(__unix__)

#include <errno.h>
#include <unistd.h>

#endif

int tcpip::get_last_error() {
#ifdef _WIN32
    return WSAGetLastError();
#elif defined(__unix__)
    return errno;
#endif
}

void tcpip::close_socket(socket_t socket) {
#ifdef _WIN32
    closesocket(socket);
#elif defined(__unix__)
    close(socket);
#endif
}
