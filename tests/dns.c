/*
 * tests/dns.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#include "tests.h"

#include "../src/dns.h"

START_TEST (dns_qname) {
	char in[128];
	char out[128];

	strncpy ( in, "sub.domain.example.com\0", 127);

	ck_assert_int_gt( fqdn_to_qname (in,128,out,128), 0 );
	ck_assert_int_ge( qname_check(out,128), 0 );
	ck_assert_int_gt( qname_to_fqdn (out,128,in,128), 0);
} END_TEST

START_TEST (dns_qname_fuzz) {
	FILE* urand = fopen ("/dev/urandom", "r");
	char rand[128];
	const unsigned long int limit = 1000000;
	unsigned long int valid_cnt = 0;
	unsigned long int i;

	if ( !urand )
		ck_abort_msg("Failed to open /dev/urandom");

	for ( i = 0; i < limit; i++) {
		if (fread (rand, 128, 1, urand) > 0) {
			if ( qname_check(rand, 128) > 0 ) {
				valid_cnt++;
			}
		}
	}

	ck_assert_float_le( (float)valid_cnt / (float)limit * 100, 10);
}

TCase* test_dns(void) {
	TCase *tc = tcase_create("DNS");

	tcase_add_test(tc, dns_qname);
	tcase_add_test(tc, dns_qname_fuzz);

	return tc;
}
