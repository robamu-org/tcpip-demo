#ifndef UTILITY_H_
#define UTILITY_H_

#include "definitions.h"
#include <stddef.h>

/**
 * Retrieve the name of the current OS.
 * @param name_buf  Name will be written to this buffer.
 * @param buf_len   Maximum allowed bytes to be written to the buffer.
 * @return Number of bytes written into the name buffer.
 */
int get_os_name(char* name_buf, size_t buf_len);
int get_last_socket_error();

int setup_sockets(void* thread_arg, const char** server_address, const char** server_port);
int cleanup_sockets(socket_t* socket_array, size_t array_len);

#endif /* UTILITY_H_ */
