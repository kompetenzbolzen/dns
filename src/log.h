/* log.h
 * (c) Jonas Gunz, 2019
 * License: MIT
 * */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#define _LOG_ERRNO	1
#define _LOG_ERROR	2
#define _LOG_WARNING	3
#define _LOG_NOTE	4
#define _LOG_DEBUG	5

extern unsigned int log_loglevel;
extern int log_fd;

extern const char* log_loglevel_str[6];

#define LOGPRINTF(l,...) {\
	if((l) <= log_loglevel){\
		time_t current = time (NULL);\
		struct tm *tma = localtime(&current);\
		char* date = asctime(tma);\
		date[strlen(date) - 1] = '\0';\
		printf("[%s] %s: ", date,  log_loglevel_str[(l)]);\
		if((l) == _LOG_ERRNO)\
			printf("%s: ", strerror(errno));\
		if((l) == _LOG_DEBUG)\
			printf("%s:%d: ", __FILE__, __LINE__);\
		printf(__VA_ARGS__);\
		printf("\n");\
		fsync(STDOUT_FILENO);\
	}\
}

// DEBUG Wrapper around LOGPRINTF wich is only compiled in in
// _DEBUG mode for performance

#ifdef _DEBUG
#define DEBUG(...) { LOGPRINTF(_LOG_DEBUG, __VA_ARGS__); }
#else
#define DEBUG(...) { }
#endif


/**
 * Opens logfile, writes filedes to _fd
 * */
int log_init_file(char* _file, unsigned int _verbosity);

/**
 * Configures LOG macros for stdout
 */
int log_init_stdout(unsigned int _verbosity);

int log_close();
