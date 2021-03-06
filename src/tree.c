/*
 * tree.c
 * (c) 2019 Jonas Gunz
 * License: MIT
 * */

#include "tree.h"

static int string_compare ( const char* _1, const char* _2 );

/**
 * ignore-case alphabetical string compare
 * returns:
 * 0  :: _1 == _2
 * -1 :: _1 <  _2
 * +1 :: _1 >  _2
 * */
static int string_compare ( const char* _1, const char* _2 )
{
	int i;

	if ( !_1 || !_2 )
		return 99;

	for (i = 0; _1[i] && _2[i]; i++) {
		char c1 = _1[i];
		char c2 = _2[i];

		/* Convert to uppercase */
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

	/* TODO not so great */
	return 99;
}

int tree_insert ( tree_node_t** _root, char* _key, void* _data )
{
	tree_node_t** node = _root;

	while( *node ) {
		int ret = string_compare ( (*node)->key, _key );
		if ( ret > 0 ) {
			node = & (*node)->above;
		} else if ( ret < 0 ) {
			node = & (*node)->below;
		} else { /* Already exists */
			return 1;
		}
	}

	*node = malloc (sizeof(**node));
	if( ! *node )
		return 2;
	memset ( *node, 0, sizeof(**node) );

	(*node)->key  = _key;
	(*node)->data = _data;

	return 0;
}

int tree_balanced_insert ( tree_node_t** _root, void*  _data[], char* _key[], unsigned int _len)
{
	unsigned int i, o, n, virtual_len, indices_cnt;
	unsigned int* indices;

	indices = NULL;
	indices_cnt = 0;

	/*
	 * n is the smallest n, for which 2^(n+1) - 1 >= _len,
	 * thus describes the minimal tree depth required to store
	 * _len amount of elements
	 */
	for (n = 0; pow( 2, n+1 ) - 1 < _len; n++){}

	/* The maximum size of a tree with depth n; */
	virtual_len = pow( 2, n+1 ) - 1;

	indices = malloc( virtual_len * sizeof(unsigned int) );

	if(!indices)
		return -1;


	LOGPRINTF(_LOG_DEBUG, "Elements: %u Rounded size: %u Optimal depth: %u", _len, virtual_len, n);

	/*
	 * Creates the series
	 * 1/2, 1/4, 3/4, 1/8, 3/8, 5/8, 7/8, ...
	 */
	for (i = 0; i <= n+1; i++) {
		unsigned int pow_2_i = pow(2,i);
		for (o = 1; o < pow(2,i); o+=2) {
			indices[ indices_cnt++ ] = virtual_len * o / pow_2_i;
		}
	}

	for ( i = 0; i < virtual_len; i++ ) {
		if ( indices[i] < _len ) {
			if (tree_insert ( _root, _key[indices[i]], _data[indices[i]] )){ 
				LOGPRINTF(_LOG_WARNING, "tree_insert failed on \"%s\". Double?", _key[indices[i]]);
			}
		}
	}

	free( indices );
	indices = NULL;

	return 0;
}

int tree_destroy ( tree_node_t** _root, uint8_t _options )
{
	/* Not efficient, but this code is for testing only, anyways. */
	unsigned int max_depth = 0;
	unsigned int node_cnt = 0;
	while(*_root)
	{
		tree_node_t** node = _root;
		unsigned int depth = 0;

		while( (*node)->above || (*node)->below ) {
			node= (*node)->above ? & (*node)->above : & (*node)->below ;
			depth ++;
		}

		if (_options & _TREE_FREE_DATA)
			free( (*node)->data );
		if (_options & _TREE_FREE_KEY)
			free( (*node)->key );

		if ( depth > max_depth )
			max_depth = depth;

		free ( *node );
		*node = NULL;
		node_cnt ++;
	}

	LOGPRINTF(_LOG_DEBUG, "%i nodes deleted. Max depth %i", node_cnt, max_depth);

	return 0;
}

void* tree_get ( tree_node_t** _root, const char* _query )
{
	tree_node_t** node = _root;

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

	return *node ? (*node)->data : NULL;
}
