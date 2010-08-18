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
/**
 * @file sarray_test.c
 *
 * Test application for the sorted array implementation.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rtrace_testsuite.h"

#include "rtrace-resolve/sarray.h"
#include "common/utils.h"

#define SARRAY_LIMIT	5

RT_INIT();

typedef struct {
	int id;
	char* name;
} node_t;

/**
 * Compares two nodes by their id.
 * @param node1
 * @param node2
 * @return
 */
static long compare_nodes(node_t* node1, node_t* node2)
{
	return node1->id - node2->id;
}

/**
 * Frees node and its resources.
 * @param node
 */
static void free_node(node_t* node)
{
	if (node->name) free(node->name);
	free(node);
}

/**
 * Creates a new node.
 * @param id
 * @param name
 * @return
 */
static node_t* new_node(int id, const char* name)
{
	node_t* node = (node_t*)malloc_a(sizeof(node_t));
	node->id = id;
	node->name = strdup_a(name);
	return node;
}

/**
 * Verifies if node matches the specified properties.
 * @param node
 * @param id
 * @param name
 * @return
 */
static int verify_node(node_t* node, int id, const char* name)
{
	RT_ASSERT_EX(node->id == id, "(%d ? %d)", node->id, id);
	RT_ASSERT_EX(!strcmp(node->name, name), "(%s ? %s)", node->name, name);
	return RT_OK;
}

/**
 * Verifies if the array matches the specified template.
 * @param[in] array
 * @param[in] nodes   the template to match.
 * @param[in] size    the number of items in the template.
 * @return
 */
static int verify_array(sarray_t* array, node_t* nodes, int size)
{
	int i;
	node_t** node = (node_t**)array->data;
	for (i = 0; i < size; i++) {
		RT_ASSERT(verify_node(*node++, nodes[i].id, nodes[i].name) == RT_OK);
	}
	return RT_OK;
}

/**
 * Initialization test case.
 */
RT_CASE(initialize)
{
	sarray_t array;
	sarray_init(&array, SARRAY_LIMIT, (op_binary_t)compare_nodes);
	RT_ASSERT(array.size == 0);
	RT_ASSERT(array.limit == SARRAY_LIMIT);
	RT_ASSERT(array.data != NULL);
	RT_ASSERT(array.do_compare == (op_binary_t)compare_nodes);
	sarray_free(&array, (op_unary_t)free_node);
	return RT_OK;
}


RT_CASE(add_node)
{
	node_t array_template1[] = {
			{.id = 5, .name = "five"},
		};
	node_t array_template2[] = {
			{.id = 3, .name = "three"},
			{.id = 5, .name = "five"},
		};
	node_t array_template3[] = {
			{.id = 3, .name = "three"},
			{.id = 5, .name = "five"},
			{.id = 6, .name = "six"},
		};
	node_t array_template4[] = {
			{.id = 3, .name = "three"},
			{.id = 4, .name = "four"},
			{.id = 5, .name = "five"},
			{.id = 6, .name = "six"},
		};
	node_t array_template5[] = {
			{.id = 1, .name = "one"},
			{.id = 3, .name = "three"},
			{.id = 4, .name = "four"},
			{.id = 5, .name = "five"},
			{.id = 6, .name = "six"},
		};
	node_t array_template6[] = {
			{.id = 1, .name = "one"},
			{.id = 2, .name = "two"},
			{.id = 3, .name = "three"},
			{.id = 4, .name = "four"},
			{.id = 5, .name = "five"},
			{.id = 6, .name = "six"},
		};
	sarray_t array;
	sarray_init(&array, SARRAY_LIMIT, (op_binary_t)compare_nodes);

	sarray_add(&array, new_node(5, "five"));
	RT_ASSERT(verify_array(&array, array_template1, RT_SIZEOF(array_template1)) == RT_OK);
	sarray_add(&array, new_node(3, "three"));
	RT_ASSERT(verify_array(&array, array_template2, RT_SIZEOF(array_template2)) == RT_OK);
	sarray_add(&array, new_node(6, "six"));
	RT_ASSERT(verify_array(&array, array_template3, RT_SIZEOF(array_template3)) == RT_OK);
	sarray_add(&array, new_node(4, "four"));
	RT_ASSERT(verify_array(&array, array_template4, RT_SIZEOF(array_template4)) == RT_OK);
	sarray_add(&array, new_node(1, "one"));
	RT_ASSERT(verify_array(&array, array_template5, RT_SIZEOF(array_template5)) == RT_OK);
	sarray_add(&array, new_node(2, "two"));
	RT_ASSERT(verify_array(&array, array_template6, RT_SIZEOF(array_template6)) == RT_OK);
	sarray_free(&array, (op_unary_t)free_node);
	return RT_OK;
}

/**
 * Node lookup test case.
 */
RT_CASE(find_node)
{
	sarray_t array;
	sarray_init(&array, SARRAY_LIMIT, (op_binary_t)compare_nodes);

	sarray_add(&array, new_node(5, "five"));
	sarray_add(&array, new_node(3, "three"));
	sarray_add(&array, new_node(6, "six"));
	sarray_add(&array, new_node(4, "four"));
	sarray_add(&array, new_node(1, "one"));

	node_t find_templates_invalid[] = {
			{.id = 0},
			{.id = 2},
			{.id = 10},
		};
	node_t find_templates_valid[] = {
			{.id = 1, .name = "one"},
			{.id = 3, .name = "three"},
			{.id = 5, .name = "five"},
			{.id = 6, .name = "six"},
		};
	int i;
	for (i = 0; i < RT_SIZEOF(find_templates_invalid); i++) {
		node_t* node = sarray_find(&array, &find_templates_invalid[i]);
		RT_ASSERT_EX(node == NULL, "id=%d", find_templates_invalid[i].id);
	}
	for (i = 0; i < RT_SIZEOF(find_templates_valid); i++) {
		node_t* node = sarray_find(&array, &find_templates_valid[i]);
		RT_ASSERT_EX(node != NULL, "id=%d", find_templates_valid[i].id);
		RT_ASSERT_EX(verify_node(node, find_templates_valid[i].id, find_templates_valid[i].name) == RT_OK, "id=%d", find_templates_valid[i].id);
	}
	sarray_free(&array, (op_unary_t)free_node);
	return RT_OK;
}

/**
 * Counter for unary iteration tests
 */
static int unary_counter;

/**
 * Counts node id summary for unary iteration tests.
 * @param node
 * @return
 */
static long count_nodes_unary(node_t* node)
{
	unary_counter += node->id;
}

/**
 * Unary operation iteration test case
 */
RT_CASE(iterate_unary)
{
	sarray_t array;
	sarray_init(&array, SARRAY_LIMIT, (op_binary_t)compare_nodes);

	sarray_add(&array, new_node(5, "five"));
	sarray_add(&array, new_node(3, "three"));
	sarray_add(&array, new_node(6, "six"));
	sarray_add(&array, new_node(4, "four"));
	sarray_add(&array, new_node(1, "one"));

	unary_counter = 0;
	sarray_foreach(&array, (op_unary_t)count_nodes_unary);
	RT_ASSERT(unary_counter == 19);

	sarray_free(&array, (op_unary_t)free_node);
	return RT_OK;
}

/**
 * Counts node id summary for binary iteration tests.
 * @param node
 * @param counter
 * @return
 */
static long count_nodes_binary(node_t* node, int* counter)
{
	*counter += node->id;
}


/**
 * Binary operation iteration test case
 */
RT_CASE(iterate_binary)
{
	sarray_t array;
	sarray_init(&array, SARRAY_LIMIT, (op_binary_t)compare_nodes);

	sarray_add(&array, new_node(5, "five"));
	sarray_add(&array, new_node(3, "three"));
	sarray_add(&array, new_node(6, "six"));
	sarray_add(&array, new_node(4, "four"));
	sarray_add(&array, new_node(1, "one"));

	int counter = 0;
	sarray_foreach2(&array, (op_binary_t)count_nodes_binary, (void*)&counter);
	RT_ASSERT(counter == 19);

	sarray_free(&array, (op_unary_t)free_node);
	return RT_OK;
}

/**
 *
 * @return
 */
int main()
{

	RT_START("sarray");

	RT_RUN_CASE(initialize);
	RT_RUN_CASE(add_node);
	RT_RUN_CASE(find_node);
	RT_RUN_CASE(iterate_unary);
	RT_RUN_CASE(iterate_binary);

	return 0;
}
