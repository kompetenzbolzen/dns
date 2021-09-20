/*
 * tests/zonefile.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#include "tests.h"

#include "../src/zonefile.h"

START_TEST (test_zonefile_string_split) {
	int i, len;
	const char* const refstr = "this is a test";
	char* str;
	unsigned long str_len;
	char* parts[4];
	char* expected_parts[4] = {
		"this", "is", "a", "test"
	};

	str_len = strlen(refstr) + 1;

	str = malloc(str_len);
	strncpy(str, refstr, str_len);

	memset(&parts, 0, sizeof(parts));

	len = zonefile_string_split(parts, 4, str, ' ');
	ck_assert_int_eq(len, 4);

	for (i=0; i<4; i++)
		ck_assert_str_eq(parts[i], expected_parts[i]);

	free(str);
} END_TEST

TCase* test_zonefile(void) {
	TCase *tc = tcase_create("DNS");

	tcase_add_test(tc, test_zonefile_string_split);

	return tc;
}
