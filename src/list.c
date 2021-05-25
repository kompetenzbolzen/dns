/*
 * src/list.c
 * (c) 2021 Jonas Gunz <himself@jonasgunz.de>
 * License: MIT
 */

#include "list.h"

int list_add( list_element_t** _root, void* _data ) {
	list_element_t** iter = _root;
	list_element_t* new_element = malloc( sizeof(list_element_t) );

	new_element->data = _data;
	new_element->next = NULL;

	if( !new_element )
		return 1;

	while(*iter)
		iter = & (*iter)->next;

	*iter = new_element;

	return 0;
}

int list_sort ( list_element_t** _root ) {
	return -1;
}

int list_length ( list_element_t* _root ) {
	return -1;
}

void* list_pop_front ( list_element_t** _root ) {
	list_element_t* old = NULL;
	void* old_data = NULL;

	if( !*_root )
		return NULL;

	old = *_root;
	old_data = old->data;

	*_root = (*_root)->next;

	free ( old );

	return old_data;
}
