/*
 * tests/record.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#include "tests.h"

#include "../src/record.h"

START_TEST (test_records_from_string) {
	ck_assert_int_eq(record_class_from_str("IN"), 1);
	ck_assert_int_eq(record_class_from_str("hs"), 4);
	ck_assert_int_eq(record_class_from_str("invalid"), 0);

	ck_assert_int_eq(record_type_from_str("A"), 1);
	ck_assert_int_eq(record_type_from_str("txt"), 16);
	ck_assert_int_eq(record_type_from_str("invalid"), 0);
} END_TEST

START_TEST (test_records_handler) {
	void* rdata = NULL;
	ssize_t len;

	len = record_rdata_from_str(&rdata, "69.69.69.68", record_class_from_str("A"));
	ck_assert_int_eq(len,4);
	ck_assert_uint_eq(*((uint32_t*)rdata),0x45454544);

	free(rdata);
}

TCase* test_record(void) {
	TCase *tc = tcase_create("record");

	tcase_add_test(tc, test_records_from_string);
	tcase_add_test(tc, test_records_handler);

	return tc;
}
