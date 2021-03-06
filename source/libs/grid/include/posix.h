/**
	The Grid Core Library
 */

/**
	The most common definition
	Yaosihai
 */

#ifndef __CRT_POSIX_H
#define __CRT_POSIX_H

#include "crt.h"

/* End char size for string */
#define POSIX_END_CHAR_SIZE 1

/* Used when open failed for posix file */
#define POSIX_INVALID_FD (-1)

/* Return value */
#define POSIX_FAILED (-1)
#define POSIX_SUCCESS 0

void *posix_fd_translate(int fd);
void posix_fd_deinit();
bool posix_fd_init();
bool posix_fd_deallocate(int fd);
int posix_fd_allocate(void *object);

#endif
