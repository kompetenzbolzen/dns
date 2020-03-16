/*
 * tree.h
 * (c) 2019 Jonas Gunz
 * License: MIT
 * */

#pragma once

#include <stdlib.h>
#include <stdint.h>

struct tree_node {
	char* key;
	void* data;
	struct tree_node* above;
	struct tree_node* below;
};

int tree_insert	( struct tree_node** _root, char* _key, void* _data );

int tree_balance( struct tree_node** _root );

void* tree_get	( struct tree_node** _root, char* _query );

int tree_destroy( struct tree_node** _root );

/**
 * ignore-case alphabetical string compare
 * returns:
 * 0  :: _1 == _2
 * -1 :: _1 <  _2
 * +1 :: _1 >  _2
 * */
int string_compare ( char* _1, char* _2 );

