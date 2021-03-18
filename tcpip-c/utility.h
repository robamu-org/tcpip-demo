#ifndef UTILITY_H_
#define UTILITY_H_

#include <stddef.h>

/**
 * Retrieve the name of the current OS.
 * @param name_buf  Name will be written to this buffer.
 * @param buf_len   Maximum allowed bytes to be written to the buffer.
 * @return Number of bytes written into the name buffer.
 */
int get_os_name(char* name_buf, size_t buf_len);


#endif /* UTILITY_H_ */
