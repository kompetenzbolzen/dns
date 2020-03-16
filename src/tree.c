/*
 * tree.c
 * (c) 2019 Jonas Gunz
 * License: MIT
 * */

#include "tree.h"


int tree_insert ( struct tree_node** _root, char* _key, void* _data )
{
	struct tree_node** node = _root;

	while( *node ) {
		int ret = string_compare ( (*node)->key, _key );
		if ( ret > 0 ) {
			node = & (*node)->above;
		} else if ( ret < 0 ) {
			node = & (*node)->below;
		} else { //Already exists
			return 1;
		}
	}

	*node = malloc (sizeof(typeof(**node)));
	if( ! *node )
		return 1;

	(*node)->key  = _key;
	(*node)->data = _data;

	return 0;
}

int tree_balance ( struct tree_node** _root )
{
	return 1;
}

int tree_destroy ( struct tree_node** _root )
{
	return 1;
}

int string_compare ( char* _1, char* _2 )
{
	if ( !_1 || !_2 )
		return 99;
	int i;
	for (i = 0; _1[i] && _2[i]; i++) {
		char c1 = _1[i];
		char c2 = _2[i];

		//Convert to uppercase
		if ( c1 >= 97 && c1 <= 122 )
			c1 -= 32;
		if ( c2 >= 97 && c2 <= 122 )
			c2 -= 32;

		if (c1 > c2)
			return 1;
		if (c1 < c2)
			return -1;
	}

	if ( _1[i] == _2[i] )
		return 0;
	if ( _1[i] )
		return 1;
	if ( _2[i] )
		return -1;

	//TODO WARN may reach end of non-void function
}

void* tree_get ( struct  tree_node** _root, char* _query )
{
	struct tree_node** node = _root;

	while(*node) {
		int ret = string_compare ( (*node)->key, _query );
		if ( ret > 0 ) {
			node = & (*node)->above;
		} else if ( ret < 0 ) {
			node = & (*node)->below;
		} else {
			break;
		}
	}

	return *node ? (*node)->key : NULL;

	return 0;
}
