/*
 * zonefile.h
 * (c) 2019 Jonas Gunz
 * License: MIT
 * */

#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "tree.h"

struct record_entry {
	char* name;
	uint32_t ttl;
	uint16_t class;
	uint16_t type;
	uint16_t rdlength;
	char* rd;
};

struct record_node {
	struct record_entry* rr;
	struct record_node* below;
	struct record_node* above;
};

/**
 * */
int zonefile_parse ( char* _filename, struct record_node* _dns_zone );

int zonefile_query ( char* _hostname, struct record_entry* _entry );

