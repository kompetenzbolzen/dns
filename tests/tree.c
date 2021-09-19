/*
 * tests/tree.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#include "tests.h"

#include "../src/tree.h"

#include <check.h>

START_TEST(tree_basic) {
	unsigned int  i, j;

	unsigned const int len = pow ( 'z' - 'a' + 1, 2);
	unsigned int len_cnt = 0;
	char** keys;
	char** data;

	struct tree_node* root = NULL;

	keys = malloc(len * sizeof(char*));
	data = malloc(len * sizeof(char*));

	for ( i = 'a'; i <= 'z'; i++ ) {
		for ( j = 'a'; j <= 'z'; j++ ) {
			keys[len_cnt] = malloc (3);
			keys[len_cnt][0] = (char)i;
			keys[len_cnt][1] = (char)j;
			keys[len_cnt][2] = 0;

			data[len_cnt] = malloc(10);
			snprintf( data[len_cnt], 10, "N%i", len_cnt );

			len_cnt ++;
		}
	}

	ck_assert_int_eq(tree_balanced_insert( &root, (void**)data, keys, len ),0);

	for ( i = 0; i < len; i++ ) {
		ck_assert_str_eq( tree_get(&root, keys[i]), data[i] );
	}

	ck_assert_int_eq( tree_destroy (&root, _TREE_FREE_DATA | _TREE_FREE_KEY), 0 );

	free(keys);
	free(data);
} END_TEST

TCase* test_tree(void) {
	TCase *tc = tcase_create("Tree");

	tcase_add_test(tc, tree_basic);

	return tc;
}
