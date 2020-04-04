/* server.h
 * (c) Jonas Gunz, 2020
 * License: MIT
 * */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <signal.h>

#include "dns.h"
#include "log.h"

#define UDP_BUFFER_LEN 512

static int sock_server;

void run_dns_server ( void );

int handle_connection (	int _socket,
			struct sockaddr_in *sockaddr_client,
			socklen_t sockaddr_client_len,
			char* _buffer,
			int _bufflen );

void signal_term ( );
