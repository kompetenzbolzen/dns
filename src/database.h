/*
 * src/database.h
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#pragma once

#include <stdlib.h>

#include "tree.h"
#include "log.h"

/* TODO remove */
#include "dns.h"

/*
 * Structure
 *
 * |-CLASS_IN
 * | |-RR_A tree
 * | |-RR_AAAA tree
 * | |-...
 * |-CLASS_CS
 * | |-...
 * |-...
 *
 * !! Always substract 1 from CLASS and RR Types, they start with 1
 *
 * Anything other than IN is probably never needed, but easier to do now than later.
 *
 * Data format in tree void*
 * 0     4     6         6+len
 * | ttl | len | data ... |
 * ttl: uint32_t
 * len: uint16_t
 *
 */

#define DB_CLASS_LEN	3
#define DB_RR_LEN	32

typedef struct database {
	tree_node_t*** zone;
} database_t;

typedef struct database_rdata {
	char*    rdata;
	uint16_t rdlen;
	uint32_t ttl;
} database_rdata_t;

int database_init ( database_t* _database );

int database_destroy ( database_t* _database );

int database_query (
	database_rdata_t* _rdata,
	database_t* _database,
	const char* _qname,
	int _qname_len,
	uint16_t _qtype,
	uint16_t _qclass
);
