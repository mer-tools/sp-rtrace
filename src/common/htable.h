/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010 by Nokia Corporation
 *
 * Contact: Eero Tamminen <eero.tamminen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02r10-1301 USA
 */

#ifndef HTABLE_H
#define HTABLE_H

#include "dlist.h"

/**
 * @file htable.h
 *
 * Hash table implementation.
 *
 * This list can be used only with data structures having
 * htable_node_t structure as the first member.
 * Also this hash table  list doesn't manage the resources
 * used by its nodes - the user is responsible for
 * allocating/freeing them. */

/**
 * Hash value caching structure.
 * This structure must be the first member of any
 * structure that is stored in the hash table.
 */
typedef struct htable_node_t {
    /* dlist_t support */
    dlist_node_t node;
    /* the owner bucket */
	dlist_t* bucket;
} htable_node_t;

/**
 * The hash table.
 */
typedef struct htable_t {
	/* the hash table buckets */
	dlist_t* buckets;

	/* the hash table size */
	int size;

	/* the node comparison function */
	op_binary_t do_compare;
	/* hash value calculation function */
	op_unary_t do_calc_hash;

} htable_t;


/**
 * Initializes hash table.
 *
 * @param[in] ht            the hash table to initialize.
 * @param[in] size          the hash table size (number of buckets).
 * @param[in] do_calc_hash  the hash function.
 * @param[in] do_compare    the comparison function.
 * @return                  0 - success.
 *
 */
int htable_init(htable_t* ht, int size, op_unary_t do_calc_hash, op_binary_t do_compare);


/**
 * Frees the hash table.
 *
 * If the free_node function is specified, it's called for each
 * hash table node to release resources used by the node itself.
 * @param[in] ht          the hash table.
 * @param[in] free_node   the node freeing function. Can be NULL.
 * @return
 */
void htable_free(htable_t* ht, op_unary_t free_node);

/**
 * Creates and initializes new hash table node.
 *
 * @param[in] size  the node size.
 * @return          the created node.
 */
void* htable_create_node(int size);

/**
 * Finds a node matching the specified data.
 *
 * @param[in] ht    the hash table.
 * @param[in] data  the data template to match.
 * @return          the located node or NULL.
 */
void* htable_find(htable_t* ht, void* data);

/**
 * Removes the specified node from the hash table.
 *
 * Note that the node itself is not freed - the user is responsible for
 * node allocation/freeing.
 * @param[in] node  the template node, containing data to search for.
 * @return
 */
void htable_remove_node(void* node);


/**
 * Stores data node into hash table.
 *
 * If hash table already contains matching node,
 * it is replaced and the old node returned.
 * @param[in] ht    the hash table.
 * @param[in] rec   the node to store.
 * @return          the old node (if such existed) or NULL.
 */
void* htable_store(htable_t* ht, void* rec);


/**
 * Calls a function for all nodes in table.
 *
 * @param[in] ht       the hash table.
 * @param[in] do_what  the function to call.
 * @return
 */
void htable_foreach(htable_t* ht, op_unary_t do_what);

/**
 * Calls a function for all nodes in table.
 *
 * This function is similar to htable_foreach, but additionally
 * it passes custom user data to the called function.
 * @param[in] ht       the hash table.
 * @param[in] do_what  the function to call.
 * @param[in] data     the user data to pass to do_what function.
 * @return
 */
void htable_foreach2(htable_t* ht, op_binary_t do_what, void* data);

/**
 * Dumps the contents of the hash table to stdout.
 *
 * This function is for debug purposes only.
 * TODO: remove (comment out) later.
 */
void htable_dump(htable_t* ht, op_unary_t do_print_node);


#endif
