#include "tcpipCommon.h"

#ifdef _WIN32
#include "winsock2.h"
#elif defined(__unix__)
#include <errno.h>
#endif

int tcpip::get_last_error() {
#ifdef _WIN32
    return WSAGetLastError();
#elif defined(__unix__)
    return errno;
#endif
}
