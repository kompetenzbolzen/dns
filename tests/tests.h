/*
 * tests/tests.h
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#pragma once

#include <check.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TCase* test_dns(void);
TCase* test_tree(void);
TCase* test_zonefile(void);
