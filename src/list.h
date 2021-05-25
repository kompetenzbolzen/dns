/*
 * src/list.h
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct list_element list_element_t;

struct list_element {
	void* data;
	list_element_t* next;
};

int list_add( list_element_t** _root, void* _data );

int list_sort ( list_element_t** _root );

int list_length ( list_element_t* _root );

void* list_pop_front ( list_element_t** _root );
