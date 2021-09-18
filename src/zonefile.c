/*
 * src/zonefile.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#include "zonefile.h"

int zonefile_parse_line(database_t *_database, char *_line) {
	unsigned int i, o, start;
	char *parts[5];

	/* Does this work? */
	memset(&parts, 0, sizeof(parts));

	start = 0;
	for ( i=0; i < 4; i++ ) {
		for ( o=start; _line[o] && _line[o] != ' '; o++ );

		parts[i] = &_line[start];

		if(!_line[o])
			break;

		_line[o] = '\0';

		start = o+1;
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

	while(!feof(zfile)) {
		line_cnt ++;
		line_len = getline(&line, 0, zfile);

		/* getline includes the line break. ONLY UNIX!! */
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

