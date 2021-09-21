/*
 * tests/record.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#include "tests.h"

#include "../src/record.h"

static char* types[] = {
	"A",
	"NS",
	"TXT"
};
static char* data[] = {
	"1.2.3.4",
	"ns.example.com.",
	"Lorem Ipsum"
};
static size_t record_cnt = sizeof(types) / sizeof(char*);

START_TEST (test_records_from_string) {
	ck_assert_int_eq(record_class_from_str("IN"), 1);
	ck_assert_int_eq(record_class_from_str("hs"), 4);
	ck_assert_int_eq(record_class_from_str("invalid"), 0);

	ck_assert_int_eq(record_type_from_str("A"), 1);
	ck_assert_int_eq(record_type_from_str("txt"), 16);
	ck_assert_int_eq(record_type_from_str("invalid"), 0);
} END_TEST

START_TEST (test_records_handler_a) {
	void* rdata = NULL;
	ssize_t len;

	len = record_rdata_from_str(&rdata, "69.69.69.68", record_type_from_str("A"));
	ck_assert_int_eq(len,4);
	ck_assert_uint_eq(*((uint32_t*)rdata),0x44454545);

	len = record_rdata_from_str(&rdata, "", record_type_from_str("MD"));
	ck_assert_int_lt(len,-0);

	free(rdata);
} END_TEST

START_TEST (test_records_handlers) {
	void* rdata = NULL;
	ssize_t len;

	len = record_rdata_from_str(&rdata, data[_i], record_type_from_str(types[_i]));
	ck_assert_int_gt(len, 0);
	free(rdata);
} END_TEST

TCase* test_record(void) {
	TCase *tc = tcase_create("record");

	tcase_add_test(tc, test_records_from_string);
	tcase_add_test(tc, test_records_handler_a);
	tcase_add_loop_test(tc, test_records_handlers, 0, record_cnt);

	return tc;
}
