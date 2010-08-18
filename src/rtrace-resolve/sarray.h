/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010 by Nokia Corporation
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

#ifndef SARRAY_H
#define SARRAY_H

#include "common/data_ops.h"

/**
 * @file sarray.h
 *
 * Sorted array implementation.
 *
 * Stores user managed data pointers in a sorted array, speeding
 * up data access.
 * The internal array used as data storage is doubled every time
 * when full.
 */

/**
 * The sorted array container.
 */
typedef struct sarray_t {
	/* the array data (items) */
	void** data;
	/* the current array size */
	int size;

	/* the size limit */
	int limit;

	/* the comparison function */
	op_binary_t do_compare;
} sarray_t;


/**
 * Initializes array.
 *
 * @param[in] array       the array.
 * @param[in] limit       the initial size limit.
 * @param[in] do_compare  the item comparison function.
 * @return                0 if successful.
 *                        -ENOMEM when failed to allocate data container.
 */
int sarray_init(sarray_t* array, int limit, op_binary_t do_compare);

/**
 * Frees the sorted array.
 *
 * If the @p free_item function is specified it will be called for
 * each item to free resources used by the items themselves.
 * @param[in] array      the sorted array.
 * @param[in] free_item  an optional item freeing function.
 *                       Can be NULL.
 * return
 */
void sarray_free(sarray_t* array, op_unary_t free_item);

/**
 * Adds new item to the array.
 *
 * @param[in] array   the array.
 * @param[in] item    the item to add.
 * @return
 */
void sarray_add(sarray_t* array, void* item);


/**
 * Finds a item matching the specified data.
 *
 * @param[in] array    the array.
 * @param[in] data     the matching data template..
 * @return             the found item or NULL.
 */
void* sarray_find(sarray_t* array, void* data);

/**
 * Calls a function for all items in array.
 *
 * @param[in] ht       the array.
 * @param[in] do_what  the function to call.
 * @return
 */
void sarray_foreach(sarray_t* array, op_unary_t do_what);

/**
 * Calls a function for all array items.
 *
 * @param[in] ht       the sorted array.
 * @param[in] do_what  the function to call.
 * @param[in] data     the user data to pass to @p do_what function.
 * @return
 */
void sarray_foreach2(sarray_t* array, op_binary_t do_what, void* data);

#endif

