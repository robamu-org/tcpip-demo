#include "utility.h"

#include <string>

std::mutex print_mutex;

std::string get_os_name() {
#ifdef _WIN32
    return "Windows";
#elif __APPLE__ || __MACH__
    return "Mac OSX";
#elif __linux__
    return "Linux";
#elif __FreeBSD__
    return "FreeBSD";
#elif __unix || __unix__
    return "Unix";
#else
    return "Other";
#endif

}

struct std::tm* get_local_time() {
    std::time_t t = std::time(nullptr);
    return std::localtime(&t);
}

std::unique_lock<std::mutex> print_guard() {
    return std::move(std::unique_lock<std::mutex>(print_mutex));
}
