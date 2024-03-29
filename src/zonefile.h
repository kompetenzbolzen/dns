/*
 * src/zonefile.h
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "database.h"
#include "log.h"
#include "record.h"

/**
 * NOT COMPATIBLE WITH STANDARD ZONEFILES!
 * use FIXED format
 * <name> <ttl> <class> <type> <value>
 */
int zonefile_to_database (database_t *_database, char* _file);

int zonefile_parse_line(database_t *_database, char *_line);

/* Chnages the original string! */
int zonefile_string_split(char* _parts[], ssize_t _max, char* _str, char _delim);
