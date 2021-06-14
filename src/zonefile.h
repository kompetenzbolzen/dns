/*
 * src/zonefile.h
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#pragma once

#include <stdio.h>

int zonefile_parse_to_list (void** _list, char* _file);
