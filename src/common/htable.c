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
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "htable.h"
#include "utils.h"

/**
 * Returns bucket for the specified node.
 *
 * This function returns the owner bucket for nodes in hash table.
 * For nodes not in the table it will first calculate it's hash
 * value and then return the bucket of the calculated hash value.
 * @param[in] ht    the hash table.
 * @param[in] node  the node.
 * @return          bucket of the specified node.
 */
static dlist_t* get_bucket(htable_t* ht, htable_node_t* node)
{
	if (node->bucket == NULL) {
		int hash = ht->do_calc_hash(node);
		if (hash >= ht->size) hash %= ht->size;
		node->bucket = &ht->buckets[hash];
	}
	return node->bucket;
}

/*
 * Public API implementation
 */
int htable_init(htable_t* ht, int size, op_unary_t do_calc_hash, op_binary_t do_compare)
{
	/* because dlist_init simply assigns NULL to tail/head references,
	 * we can just initialize the dlist_t array with zeroes without
	 * explicitly calling dlist_init for each array item. */
	ht->buckets = (dlist_t*)calloc_a(size, sizeof(dlist_t));
	ht->do_compare = do_compare;
	ht->do_calc_hash = do_calc_hash;
	ht->size = size;
	return 0;
}

void htable_free(htable_t* ht, op_unary_t free_node)
{
	int i;
	for (i = 0; i < ht->size; i++) {
		dlist_free(&ht->buckets[i], free_node);
	}
	free(ht->buckets);
}

void* htable_create_node(int size)
{
    htable_node_t* node = (htable_node_t*)dlist_create_node(size);
    node->bucket = 0;
    return node;
}

void* htable_find(htable_t* ht, void* data)
{
	dlist_node_t* node = dlist_find(get_bucket(ht, data), data, ht->do_compare);
	return node;
}

void htable_remove(htable_t* ht, void* node)
{
	htable_node_t* tnode = (htable_node_t*)node;
	if (tnode->bucket) {
	    dlist_remove(tnode->bucket, node);
	}
}


void* htable_store(htable_t* ht, void* node)
{
	htable_node_t* tnode = (htable_node_t*)node;
	dlist_t* bucket = get_bucket(ht, tnode);
	dlist_node_t* old_node = dlist_find(bucket, node, ht->do_compare);
	if (old_node) {
	    dlist_remove(bucket, old_node);
	}
	tnode->bucket = bucket;
    dlist_add(bucket, node);

	return old_node;
}

void htable_foreach(htable_t* ht, op_unary_t do_what)
{
	int i;
	for (i = 0; i < ht->size; i++) {
		dlist_foreach(&ht->buckets[i], do_what);
	}
}

void htable_foreach2(htable_t* ht, op_binary_t do_what, void* data)
{
	int i;
	for (i = 0; i < ht->size; i++) {
		dlist_foreach2(&ht->buckets[i], do_what, data);
	}
}


void htable_dump(htable_t* ht, op_unary_t do_print_node)
{
	int i;
	for (i = 0; i < ht->size; i++) {
		printf("==========\n");
		printf("[%02x]\n", i);
		dlist_t* bucket = &ht->buckets[i];
		dlist_node_t* node = bucket->head;
		while (node) {
			printf("--------\n");
			do_print_node(node);
			node = node->next;
		}
	}
}


