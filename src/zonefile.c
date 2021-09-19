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
	int parts_len;

	/* Does this work? */
	memset(&parts, 0, sizeof(parts));

	parts_len = zonefile_string_split(parts, 4, _line, ' ');
	if (parts_len != 4) {
		LOGPRINTF(_LOG_ERROR, "Incomplete");
		return -1;
	}

	/* parts is the first 5 space-seperated parts of _line */

	return -1;
}

int zonefile_to_database (database_t *_database, char* _file) {
	FILE *zfile = NULL;
	char *line = NULL;
	ssize_t line_len  = 0;
	unsigned int line_cnt = 0;

	zfile = fopen(_file, "r");
	if (!zfile) {
		LOGPRINTF(_LOG_ERRNO, "Could not open %s", _file);
		return -1;
	}

	DEBUG("Parsing zonefile %s", _file)

	while(!feof(zfile)) {
		line_cnt ++;
		line_len = getline(&line, 0, zfile);

		/* getline includes the line break. ONLY UNIX ENDINGS!! */
		if( line[line_len - 2] == '\n' )
			line[line_len - 2] = '\0';

		if ( zonefile_parse_line(_database, line) < 0) {
			LOGPRINTF(_LOG_ERROR, "Error is in line %u", line_cnt)
			return -1;
		}

		free(line);
		line = NULL;
	}

	fclose(zfile);
	return -1;
}

