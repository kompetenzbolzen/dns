/*
 * tests/database.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#include "tests.h"

#include "../src/database.h"
#include "../src/zonefile.h"
#include <check.h>

START_TEST ( test_database_parse_zonefile ) {
	int ret;
	char qname[128];
	int qname_len;
	database_t db;
	database_rdata_t data;

	qname_len = fqdn_to_qname("test.example.com.", 18, qname, 128);
	ck_assert_int_gt(qname_len, 0);

	database_init( &db );
	ck_assert_ptr_nonnull( db.zone );

	ret = zonefile_to_database( &db, "tests/zone.file" );
	ck_assert_int_eq(ret,0);

	ret = database_query( &data, &db, qname, qname_len, 1, 1 );
	ck_assert_int_eq(ret, 0);

	database_destroy(&db);
	ck_assert_ptr_null( db.zone );
} END_TEST

START_TEST (test_database_error_handling) {
	database_t db;
	database_rdata_t rdata;
	database_init(&db);

	ck_assert_int_ne(database_init(NULL), 0);
	ck_assert_int_ne(database_destroy(NULL), 0);
	ck_assert_int_ne(database_query(NULL, NULL, NULL, 0, 0, 0), 0);

	/* Record not found */
	ck_assert_int_ne(database_query(&rdata, &db, "aa", 3, 1, 1), 0);

	/* Invalid record */
	ck_assert_int_ne(database_query(&rdata, &db, "aa", 3, 1, 777), 0);
	ck_assert_int_ne(database_query(&rdata, &db, "aa", 3, 777, 1), 0);

	database_destroy(&db);
} END_TEST


TCase* test_database(void) {
	TCase *tc = tcase_create("Database");

	tcase_add_test(tc, test_database_parse_zonefile);
	tcase_add_test(tc, test_database_error_handling);

	return tc;
}
