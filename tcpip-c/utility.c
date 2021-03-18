#include "utility.h"

#include <stdio.h>

int get_os_name(char* const name_buf, size_t buf_len) {
#ifdef _WIN32
    return snprintf(name_buf, buf_len, "Windows");
#elif __APPLE__ || __MACH__
    return snprintf(name_buf, buf_len, "Mac OSX");
#elif __linux__
    return snprintf(name_buf, buf_len, "Linux");
#elif __FreeBSD__
    return snprintf(name_buf, buf_len, "FreeBSD");
#elif __unix || __unix__
    return snprintf(name_buf, buf_len, "Unix");
#else
    return snprintf(name_buf, buf_len, "Other");
#endif

}

