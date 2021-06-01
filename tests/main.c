/*
 * tests/main.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#include <check.h>

#include "tests.h"

int main() {
	Suite *s;
	SRunner *sr;
	int failed;

	s = suite_create("All Tests");

	suite_add_tcase(s, test_dns());

	sr = srunner_create(s);
	srunner_run_all(sr,CK_NORMAL);

	failed = srunner_ntests_failed(sr);

	srunner_free(sr);

	return failed;
}
