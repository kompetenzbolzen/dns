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
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <signal.h>

#include <sys/select.h>

#include "dns.h"
#include "log.h"
#include "database.h"

#define UDP_BUFFER_LEN 512

typedef struct server_config {
	char* bind_ip;
	uint16_t bind_port;
	char* zonefile;
	char* user;
} server_config_t;

void server_start ( server_config_t* _config );

void server_handle_connection ( int _socket, database_t* _zone_db );

int server_get_socket ( char* _bind_ip, uint16_t _bind_port );

void signal_term ( int _sig );
