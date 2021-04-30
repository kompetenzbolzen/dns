/* test.h
 * (c) Jonas Gunz, 2020
 * License: MIT
 * */

#pragma once

#include <stdio.h>
#include "tree.h"
#include "dns.h"
#include "database.h"

/*
 * TODO
 * * fuzz dns_parse_packet()
 * * test dns_parse_packet() with random valid data
 * * test qname_check() with random valid data
 */

#ifdef _TEST

void run_test ();

int test_tree ();

int test_dns_parsing ();

int test_dns_qname_fuzz();

int test_dns_message_fuzz();

int test_database();

#endif
