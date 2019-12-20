/*
 * zonefile.c
 * (c) 2019 Jonas Gunz
 * License: MIT
 * */

#include "zonefile.h"

int zonefile_parse ( char* _filename, struct record_node* _dns_zone )
{
	return 1;
}

int zonefile_query ( char* _hostname, struct record_entry* _entry )
{
	return 1;
}

static int tree_insert ( struct record_node* _root, struct record_entry* _node )
{
	struct record_node* node = _root;

	while(node) {
		int ret = string_compare ( node->rr->name, _node->name );
		if ( ret > 0 ) {
			node = node->above;
		} else if ( ret < 0 ) {
			node = node->below;
		} else { //Already exists
			return 1;
		}
	}

	node = malloc (sizeof(*node));
	if(!node)
		return 1;

	node->rr = _node;

	return 0;
}

static int tree_balance ( struct record_node* _root )
{
	return 1;
}

static struct record_entry* tree_get ( struct record_node* _root, char* _query )
{
	struct record_node* node = _root;

	while(node) {
		int ret = string_compare ( node->rr->name, _query );
		if ( ret > 0 ) {
			node = node->above;
		} else if ( ret < 0 ) {
			node = node->below;
		} else {
			break;
		}
	}

	return node ? node->rr : NULL;

	return 0;
}

static int tree_destroy ( struct record_node* _root )
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
}
