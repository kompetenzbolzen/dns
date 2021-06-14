/*
 * src/database.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#include "database.h"

static int database_init ( database_t* _database ) {
	unsigned int i = 0;
	size_t rr_size;

	/* Initialize 2D array of tree_node pointers, paranoia style */
	if ( !( _database->zone = malloc( sizeof( tree_node_t** ) * DB_CLASS_LEN ) ) )
		return 1;

	rr_size = sizeof( struct tree_node* ) * DB_RR_LEN;
	for ( i = 0; i < DB_CLASS_LEN; i++ ) {
		if ( !( _database->zone[i] = malloc( rr_size ) ) )
			return 1;

		memset( _database->zone[i], 0, rr_size );
	}

	return 0;
}

int database_populate ( database_t* _database, char* _zonefile ) {
	char* qname;
	int len;
	void* data;

	if ( database_init( _database ) ) {
		LOGPRINTF(_LOG_ERROR, "Failed to initialize database.");
		return 1;
	}

	/* TODO parsing */
	
	qname = malloc(32);

	len = fqdn_to_qname( "test.example.com", 17, qname, 32 );

	if ( len <= 0 )
		return 1;

	data = malloc( 10 );

	*((uint32_t*)data) = 1800;
	*((uint16_t*)(data+4)) = 4;
	*((uint32_t*)(data+6)) = 0x45454545;
	
	tree_insert( &_database->zone[0][0], qname, data );

	LOGPRINTF(_LOG_NOTE, "Database initialized and populated");

	return 0;
}

int database_destroy ( database_t* _database ) {
	unsigned int i, o;

	if ( !_database || !_database->zone )
		return 1;

	for ( i = 0; i < DB_CLASS_LEN; i++ ) {
		for ( o = 0; o < DB_RR_LEN; o++ ) {
			/* TODO should we free data and key? */
			tree_destroy( &_database->zone[i][o], _TREE_FREE_DATA | _TREE_FREE_KEY );
		}

		free( _database->zone[i] );
	}

	free( _database->zone );
	_database->zone = NULL;

	return 1;
}

int database_query (
	database_rdata_t* _rdata,
	database_t* _database,
	const char* _qname,
	int _qname_len,
	uint16_t _qtype,
	uint16_t _qclass
) {
	uint16_t type, class;
	void* data;

	/* _qtype and _qclass start at 1, so they are invalid when 0. */

	if ( !_rdata || !_database || !_qname || !_qtype || !_qclass || _qname_len <= 0 ) {
		LOGPRINTF(_LOG_ERROR, "Invalid arguments");
		return 1;
	}

	if ( _qtype >= DB_RR_LEN || _qclass >= DB_RR_LEN ) {
		LOGPRINTF(_LOG_DEBUG, "Invalid qtype/qclass");
		return 1;
	}

	_rdata->ttl   = 0;
	_rdata->rdlen = 0;
	_rdata->rdata = NULL;

	type  = _qtype  - 1;
	class = _qclass - 1;

	data = tree_get( &_database->zone[class][type], _qname );

	if ( !data ) {
		LOGPRINTF(_LOG_DEBUG, "No matching RR found");
		return 2;
	}

	_rdata->ttl   = *( (uint32_t*) data );
	_rdata->rdlen = *( (uint16_t*)(data + 4) );
	_rdata->rdata = data + 6;

	return 0;
}
