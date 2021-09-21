/*
 * src/record.h
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#pragma once

#include <stdint.h>
#include <strings.h>
#include <sys/types.h>

#include "log.h"
#include "dns.h"

uint16_t record_class_from_str(char* _str);

uint16_t record_type_from_str(char* _str);

ssize_t record_rdata_from_str(void** _rdata, char *_str, uint16_t _rdtype);
