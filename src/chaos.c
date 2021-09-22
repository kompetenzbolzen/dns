/*
 * src/chaos.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

/**
 * Randomly inject errors in library functions to check resiliency
 */

#if defined(_CHAOS) && defined(_GNU_SOURCE)

#warning "CHAOS Mode enabled"

#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

void* malloc (size_t size) {
	int fd;
	uint16_t rand;

	void* (*realmalloc)(size_t size) =
		(void* (*)(size_t size)) dlsym(RTLD_NEXT, "malloc");

	fd = open("/dev/urandom", O_RDONLY);
	read(fd, &rand, 2);
	close(fd);

	if( rand < 1000 ) {
		errno = ENOMEM;
		return NULL;
	}

	return realmalloc(size);
}

#endif
