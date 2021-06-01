#include <check.h>

START_TEST (test_example) {
	ck_abort();
} END_TEST

Suite* tests(void) {
	Suite *s;
	TCase *tc_core;

	s = suite_create("DNS Tests");

	tc_core = tcase_create("Core");

	tcase_add_test(tc_core, test_example);
	suite_add_tcase(s, tc_core);

	return s;
}

int main() {
	Suite *s;
	SRunner *sr;

	s = tests();
	sr = srunner_create(s);

	srunner_run_all(sr,CK_NORMAL);

	srunner_free(sr);

	return srunner_ntests_failed(sr);
}
