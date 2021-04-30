/*
 * src/database.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#include "database.h"

static int database_init ( struct database* _database ) {
	// Initialize 2D array of tree_node pointers, paranoia style
	if ( !( _database->zone = malloc( sizeof( struct tree_node** ) * DB_CLASS_LEN ) ) )
		return 1;

	size_t rr_size = sizeof( struct tree_node* ) * DB_RR_LEN;
	for ( unsigned int i = 0; i < DB_CLASS_LEN; i++ ) {
		if ( !( _database->zone[i] = malloc( rr_size ) ) )
			return 1;

		memset( _database->zone[i], 0, rr_size );
	}

	return 0;
}

int database_populate (
	struct database* _database,
	char* _zonefile
) {
	if ( database_init( _database ) )
		return 1;

	// TODO parsing
	
	char* qname = malloc(32);

	int len = fqdn_to_qname( "test.example.com", 17, qname, 32 );

	if ( len <= 0 )
		return 1;

	void* data = malloc( 10 );

	*((uint32_t*)data) = 1800;
	*((uint16_t*)(data+4)) = 4;
	*((uint32_t*)(data+6)) = 0x45454545;
	
	tree_insert( &_database->zone[0][0], qname, data );

	return 0;
}

int database_destroy ( struct database* _database ) {
	if ( !_database || !_database->zone )
		return 1;

	for ( unsigned int i = 0; i < DB_CLASS_LEN; i++ ) {
		for ( unsigned int o = 0; o < DB_RR_LEN; o++ ) {
			// TODO should we free data and key?
			tree_destroy( &_database->zone[i][o], _TREE_FREE_DATA | _TREE_FREE_KEY );
		}

		free( _database->zone[i] );
	}

	free( _database->zone );
	_database->zone = NULL;

	return 1;
}

int database_query (
	struct database_rdata* _rdata,
	struct database* _database,
	const char* _qname,
	int _qname_len,
	uint16_t _qtype,
	uint16_t _qclass
) {
	uint16_t type, class;

	// _qtype and _qclass start at 1, so they are invalid when 0.

	if ( !_rdata || !_database || !_qname || !_qtype || !_qclass || _qname_len <= 0 )
		return 1;

	if ( _qtype >= DB_RR_LEN || _qclass >= DB_RR_LEN )
		return 1;

	_rdata->ttl   = 0;
	_rdata->rdlen = 0;
	_rdata->rdata = NULL;

	type  = _qtype  - 1;
	class = _qclass - 1;

	void* data = tree_get( &_database->zone[class][type], _qname );

	if ( !data )
		return 2;

	_rdata->ttl   = *( (uint32_t*) data );
	_rdata->rdlen = *( (uint16_t*)(data + 4) );
	_rdata->rdata = data + 6;

	return 0;
}
