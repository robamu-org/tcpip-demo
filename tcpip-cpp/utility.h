#pragma once

#include <string>
#include <chrono>
#include <mutex>

std::string get_os_name();

/**
 * Can be used with std::put_time to print out current time with output streams.
 * @return
 */
struct std::tm* get_local_time();

/**
 * Can be used to prevent garbled output when using multiple threads.
 * @return
 */
std::unique_lock<std::mutex> print_guard();
