/*
 * tree.h
 * (c) 2019 Jonas Gunz
 * License: MIT
 * */

/**
 * Binary tree designed to serve as a static lookup table.
 * On the fly rebalancing is not intended.
 *
 * The char* _key is used as a primary key, the tree can
 * carry any generic payload in void* _data.
 * _key is case INsensitive.
 *
 * tree_balanced_insert tries to create an optimally balanced
 * tree from the given dataset. Only works correctly on an empty
 * tree.
 *
 * The root does not need special initialization.
 *
 * A query with tree_get() returns _data on an exact match,
 * NULL if no matching key is found.
 *
 * To free the memory, do not use free() on root, but
 * tree_destroy() instead. _key and _data can be freed
 * by providing _TREE_FREE_DATA | _TREE_FREE_KEY.
 *
 * Example:
 *
 *	struct tree_node* root = NULL;
 *
 *	// A String is used as data
 *	tree_insert ( &root, "one", "data one" );
 *	tree_insert ( &root, "two", "data two" );
 *
 *	printf( "%s\n", tree_get( &root, "two" ) );
 *
 *	tree_destroy ( &root, 0 );
 *
 **/

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

/* TODO remove */
#include <stdio.h>

#include "log.h"

#define _TREE_FREE_DATA 0x01
#define _TREE_FREE_KEY	0x02

typedef struct tree_node {
	char* key;
	void* data;
	struct tree_node* above;
	struct tree_node* below;
} tree_node_t;

int tree_insert	( tree_node_t** _root, char* _key, void* _data );

/**
 * Inserts the given list into the tree, achieving optimal depth.
 * Expects a sorted list.
 * */
int tree_balanced_insert ( tree_node_t** _root, void*  _data[], char* _key[], unsigned int _len);

/**
 * Returns (void*)node->data on success, NULL on failure
 * */
void* tree_get	( tree_node_t** _root, const char* _query );

int tree_destroy( tree_node_t** _root, uint8_t _options );
