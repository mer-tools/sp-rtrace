/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010-2012 by Nokia Corporation
 *
 * Contact: Eero Tamminen <eero.tamminen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */
#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "sarray.h"
#include "common/utils.h"

int sarray_init(sarray_t* array, int limit, op_binary_t do_compare)
{
	array->size = 0;
	array->limit = limit;
	array->data = malloc_a(array->limit * sizeof(void*));
	array->do_compare = do_compare;
	return 0;
}


void sarray_free(sarray_t* array, op_unary_t free_item)
{
    if (free_item) {
        sarray_foreach(array, free_item);
    }
	free(array->data);
}


void sarray_add(sarray_t* array, void* item)
{
	/* expand array if necessary */
	if (array->size == array->limit) {
		array->limit <<= 1;
		void* data = realloc_a(array->data, array->limit * sizeof(void*));
		array->data = data;
	}
	/* insert item into array keeping it sorted */
	int idx = array->size++;
	while (idx && array->do_compare(item, array->data[idx - 1]) < 0) {
		array->data[idx] = array->data[idx - 1];
		idx--;
	}
	array->data[idx] = item;
}

void* sarray_find(sarray_t* array, void* item)
{
	if (array->size) {
		int low = 0, high = array->size - 1;
		do {
			int half = (high + low) >> 1;
			long rc = array->do_compare(item, array->data[half]);
			if (!rc) return array->data[half];
			if (rc < 0) high = half - 1;
			else low = half + 1;
		} while (low <= high);
	}
	return NULL;
}

void sarray_foreach(sarray_t* array, op_unary_t do_what)
{
	int i;
	for (i = 0; i < array->size; i++) {
		do_what(array->data[i]);
	}
}

void sarray_foreach2(sarray_t* array, op_binary_t do_what, void* data)
{
	int i;
	for (i = 0; i < array->size; i++) {
		do_what(array->data[i], data);
	}
}

