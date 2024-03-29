/*
 * tests/dns.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#include "tests.h"

#include "../src/dns.h"
#include <check.h>

START_TEST (dns_qname) {
	char in[128];
	char out[128];
	char* fqdn = "sub.domain.example.com.";
	char* inval_fqdn = "is!this.domain.invalid?";

	strncpy ( in, fqdn , 127);

	ck_assert_int_gt( fqdn_to_qname (in,128,out,128), 0 );
	ck_assert_int_ge( qname_check (out,128), 0 );
	ck_assert_int_gt( qname_to_fqdn (out,128,in,128), 0);
	ck_assert_str_eq( in, fqdn );

	/* Check for working invalid protection */
	ck_assert_int_gt( fqdn_to_qname (inval_fqdn,strlen(inval_fqdn),out,128), 0 );
	ck_assert_int_ne( qname_check(out,128), 0 );
	ck_assert_int_ne( fqdn_check(inval_fqdn,strlen(inval_fqdn)), 0 );
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

	/* Do not allow more than 10% false-positives */
	ck_assert_float_le( (float)valid_cnt / (float)limit * 100, 10);
} END_TEST

START_TEST (dns_message_fuzz) {
	const unsigned long int limit = 1000000;
	unsigned long int valid_cnt = 0;
	unsigned long int i;

	struct dns_message msg;

	FILE* urand = fopen ("/dev/urandom", "r");
	char rand[128];

	if (!urand)
		ck_abort_msg("Failed to open /dev/urandom");


	for (i = 0; i < limit; i++) {
		if (fread (rand, 128, 1, urand) > 0) {
			if ( ! dns_parse_packet(rand, 128, &msg) ) {
				valid_cnt++;
			}
		}
	}

	/* Do not allow more than 10% false-positives */
	ck_assert_float_le( (float)valid_cnt / (float)limit * 100, 10);
} END_TEST

START_TEST (dns_error_handling) {
	char qname[32];
	char fqdn[32];

	strncpy(fqdn, "test.example.com", 32);

	ck_assert_int_lt( fqdn_to_qname(fqdn, 32, qname, 5), 0 );

	fqdn_to_qname(fqdn, 32, qname, 32);
	ck_assert_int_lt( qname_to_fqdn(qname, 32, fqdn, 5), 0 );
} END_TEST

TCase* test_dns(void) {
	TCase *tc = tcase_create("DNS");

	tcase_add_test(tc, dns_qname);
	tcase_add_test(tc, dns_qname_fuzz);
	tcase_add_test(tc, dns_message_fuzz);
	tcase_add_test(tc, dns_error_handling);

	return tc;
}
