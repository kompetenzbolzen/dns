/*
 * tests/database.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#include "tests.h"

#include "../src/database.h"
#include "../src/zonefile.h"

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


TCase* test_database(void) {
	TCase *tc = tcase_create("Database");

	tcase_add_test(tc, test_database_parse_zonefile);

	return tc;
}
