/* server.h
 * (c) Jonas Gunz, 2020
 * License: MIT
 * */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <signal.h>

#include "dns.h"
#include "log.h"
#include "database.h"

#define UDP_BUFFER_LEN 512

typedef struct server_config {
	char* bind_ip;
	uint16_t bind_port;
} server_config_t;

static int sock_server;

int init_socket ( );

void run_dns_server ( server_config_t* _config );

int handle_connection (	int _socket,
			struct sockaddr_in *sockaddr_client,
			socklen_t sockaddr_client_len,
			char* _buffer,
			int _bufflen,
			database_t* _zone_db );

void signal_term ( );
