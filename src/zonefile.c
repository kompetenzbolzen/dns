/*
 * src/zonefile.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#include "zonefile.h"

int zonefile_string_split(char* _parts[], ssize_t _max, char* _str, char _delim) {
	unsigned int i, o, start;

	start = 0;
	for ( i=0; i < _max; i++ ) {
		for ( o=start; _str[o] && _str[o] != ' '; o++ );

		_parts[i] = &_str[start];

		if(!_str[o])
			break;

		_str[o] = '\0';

		start = o+1;
	}

	return (int)i + 1;
}

int zonefile_parse_line(database_t *_database, char *_line) {
	char *parts[5];
	int parts_len, ret, fqdn_len;

	char* qname;
	uint32_t ttl;
	uint16_t type, class;
	void* data;
	ssize_t data_len;

	void* db_entry;

	memset(&parts, 0, sizeof(parts));

	parts_len = zonefile_string_split(parts, 5, _line, ' ');
	if (parts_len != 5) {
		LOGPRINTF(_LOG_ERROR, "Statement is incomplete");
		return -1;
	}

	fqdn_len = strlen(parts[0]);
	if ( (ret = fqdn_check(parts[0], fqdn_len)) ) {
		LOGPRINTF(_LOG_ERROR, "FQDN Contains invalid char at pos %i", ret);
		return -1;
	}
	qname = malloc( (unsigned)fqdn_len+2 );
	if ( fqdn_to_qname(parts[0], fqdn_len, qname, fqdn_len+2) < 0) {
		LOGPRINTF(_LOG_ERROR, "Failed to convert to QNAME. This is a bug.");
		return -1;
	}

	if ( !(ttl = (uint16_t) atoi(parts[1])) ) {
		LOGPRINTF(_LOG_ERROR, "Invalid TTL");
		return -1;
	}

	if ( (class = record_class_from_str(parts[2])) == 0 ) {
		LOGPRINTF(_LOG_ERROR, "Invalid class %s", parts[2]);
		return -1;
	}

	if ( (type = record_type_from_str(parts[3])) == 0 ) {
		LOGPRINTF(_LOG_ERROR, "Invalid record type %s", parts[3]);
		return -1;
	}

	if ( (data_len = record_rdata_from_str(&data, parts[4], type)) <= 0 ) {
		LOGPRINTF(_LOG_ERROR, "Invalid rdata %s", parts[4]);
		return -1;
	}

	db_entry = malloc((unsigned)data_len + 6);
	if ( !db_entry )
		goto err;
	memset(db_entry, 0, (unsigned)data_len + 6);

	DEBUG("Found %s record at %s", parts[3], parts[0]);

	*((uint32_t*)db_entry) = ttl;
	*((uint16_t*)(db_entry+4)) = (uint16_t) data_len;
	strncpy(db_entry+6, data, (unsigned)data_len);

	/* This breaks abstraction boundries! */
	if ( tree_insert( &_database->zone[class-1][type-1], qname, db_entry ) ) {
		LOGPRINTF(_LOG_ERROR, "Failed to insert in database. Does this record already exist?");
		free(db_entry);
		goto err;
	}

	free(data);
	return 0;
err:
	free(data);
	return -1;
}

/* TODO convert to creating lists of data to insert efficiently */
int zonefile_to_database (database_t *_database, char* _file) {
	FILE *zfile = NULL;
	char *line  = NULL;
	size_t llen = 0;
	ssize_t line_len  = 0;
	unsigned int line_cnt = 0;

	zfile = fopen(_file, "r");
	if (!zfile) {
		LOGPRINTF(_LOG_ERRNO, "Could not open %s", _file);
		return -1;
	}

	DEBUG("Parsing zonefile %s", _file);
	/* TODO Make resilient to evil empty lines */

	while( (line_len = getline(&line, &llen, zfile)) >= 0 ) {
		line_cnt ++;

		DEBUG("line %u, length %li, allocated %lu", line_cnt, line_len, llen);

		/* getline includes the line break. ONLY UNIX ENDINGS!! */
		if( line[line_len - 1] == '\n' )
			line[line_len - 1] = '\0';

		if ( zonefile_parse_line(_database, line) < 0) {
			LOGPRINTF(_LOG_ERROR, "Error is in line %u", line_cnt)
			return -1;
		}

		free(line);
		line = NULL;
	}

	fclose(zfile);
	return 0;
}

