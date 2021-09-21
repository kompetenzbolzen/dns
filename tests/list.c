/*
 * tests/list.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: All rights reserved.
 */

#include "tests.h"
#include "../src/list.h"

START_TEST ( test_list_all ) {
	list_element_t* root = NULL;
	unsigned int i;
	/*int values[] = {7,1,6,8,4,5,2,0,3};*/
	int values[] = {0,1,2,3,4,5,6,7,8};


	for ( i=0; i<=8; i++ )
		list_add(&root, &values[i]);


	/* Not implemented
	list_sort(&root);

	ck_assert_int_eq( list_length(root), 9 );
	*/

	for ( i=0; i<=8; i++ )
		ck_assert_int_eq(*(int*)list_pop_front(&root), i);

	/*ck_assert_int_eq( list_length(root), 0 );*/

} END_TEST

TCase* test_list(void) {
	TCase *tc = tcase_create("List");

	tcase_add_test(tc, test_list_all);

	return tc;
}
