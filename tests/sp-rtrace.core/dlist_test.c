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
 * @file dlist_test.c
 *
 * Test application for the double linked list implementation.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rtrace_testsuite.h"

#include "common/dlist.h"
#include "common/utils.h"

RT_INIT();

/**
 * dlist test node
 */
typedef struct {
	dlist_node_t node;
	int value;
	char* name;
} node_t;

/**
 * Frees the test node and its resources.
 *
 * @param[in] node
 */
static void free_node(node_t* node)
{
	free(node->name);
	free(node);
}

/**
 * Compares two test nodes.
 *
 * @param node1
 * @param node2
 * @return
 */
static long compare_nodes(node_t* node1, node_t* node2)
{
	return node1->value - node2->value;
}

/**
 * Compares node value with the specified value.
 *
 * @param node
 * @param value
 * @return
 */
static long compare_node_value(node_t* node, int value)
{
	return node->value - value;
}

/**
 * Creates new test node.
 *
 * @param[in] value  the test node value.
 * @param[in] name   the test node name.
 * @return           the test node.
 */
static node_t* new_node(int value, const char* name)
{
	node_t* node = dlist_create_node(sizeof(node_t));
	node->value = value;
	node->name = strdup_a(name);
	return node;
}

/**
 * Populates list with the given node array.
 *
 * @param[in] list
 * @param[in] node  the node array.
 * @param[in] size  the number of nodes in array.
 */
static void populate_list(dlist_t* list, node_t* node, int size)
{
	int i;
	for (i = 0; i < size; i++) {
		dlist_add(list, new_node(node->value, node->name));
		node++;
	}
}

/**
 * Populates sorted list with the given node array.
 *
 * @param[in] list
 * @param[in] node  the node array.
 * @param[in] size  the number of nodes in array.
 */
static void populate_list_sorted(dlist_t* list, node_t* node, int size)
{
	int i;
	for (i = 0; i < size; i++) {
		dlist_add_sorted(list, new_node(node->value, node->name), (op_binary_t)compare_nodes);
		node++;
	}
}


/**
 * Compares list with the given node array.
 *
 * @param[in] list
 * @param[in] node  the node array.
 * @param[in] size  the number of nodes in array.
 */
static int verify_list(dlist_t* list, node_t* node, int size)
{
	node_t* lnode = (node_t*)list->tail;
	int i;

	for (i = 0; i < size; i++) {
		RT_ASSERT_EX(lnode != NULL, "i=%d", i);
		RT_ASSERT_EX(node != NULL, "i=%d", i);
		RT_ASSERT_EX(lnode->value == node->value, "i=%d, (%d ? %d)", i, lnode->value, node->value);
		RT_ASSERT_EX(!strcmp(lnode->name, node->name), "i=%d, (%s ?  %s)", i, lnode->name, node->name);
		lnode = (node_t*)lnode->node.next;
		node++;
	}
	RT_ASSERT(lnode == NULL);
	return RT_OK;
}

/**
 * Checks if the node exists and matches the specified data.
 *
 * @param[in] node  the node to check.
 * @param[in] id    the value identifying the node.
 * @param value
 * @param name
 */
static int verify_node(node_t* node, int value, const char* name)
{
	RT_ASSERT(node != 0);
	RT_ASSERT(node->value == value);
	RT_ASSERT(!strcmp(node->name, name));
	return RT_OK;
}

/**
 * Checks if finding non-existing node properly returns NULL pointer.
 *
 * @param list
 * @param node_id
 * @return
 */
static int verify_find_failure(dlist_t* list, long node_id)
{
	node_t* node = dlist_find(list, (void*)node_id, (op_binary_t)compare_node_value);
	RT_ASSERT_EX(node == 0, "id=%ld", node_id);
	return RT_OK;
}

/**
 * Checks if finding existing node properly returns the node and it has
 * the expected properties.
 * @param list
 * @param node_id
 * @param value
 * @param name
 * @return
 */
static int verify_find_success(dlist_t* list, long node_id, int value, const char* name)
{
	node_t* node = dlist_find(list, (void*)node_id, (op_binary_t)compare_node_value);
	RT_ASSERT_EX(verify_node(node, value, name) == RT_OK, "id=%ld", node_id);
	return RT_OK;
}

/**
 * List initialization test case.
 */
RT_CASE(initialize)
{
	dlist_t list1;
	dlist_init(&list1);
	RT_ASSERT(list1.head == NULL);
	RT_ASSERT(list1.tail == NULL);
	dlist_free(&list1, NULL);

	return RT_OK;
}

/**
 * List array initialization test case.
 */
RT_CASE(initialize_array)
{
	dlist_t list[10];
	dlist_init_array(list, RT_SIZEOF(list));
	int i;
	for (i = 0; i < RT_SIZEOF(list); i++) {
		RT_ASSERT_EX(list[i].head == NULL, "i=%d", i);
		RT_ASSERT_EX(list[i].tail == NULL, "i=%d", i);
	}

	return RT_OK;
}

/**
 * Node creation test case.
 */
RT_CASE(create_node)
{
	node_t* node = dlist_create_node(sizeof(node_t));
	RT_ASSERT(node != NULL);
	RT_ASSERT(((dlist_node_t*)node)->next == NULL);
	RT_ASSERT(((dlist_node_t*)node)->prev == NULL);
	free(node);

	return RT_OK;
}

/**
 * Node adding test case.
 */
RT_CASE(add_node)
{
	node_t nodes[] = {
			{.value = 1, .name = "one"},
			{.value = 4, .name = "four"},
			{.value = 6, .name = "six"},
			{.value = 2, .name = "two"},
			{.value = 3, .name = "three"},
		};
	dlist_t list;
	dlist_init(&list);
	populate_list(&list, nodes, RT_SIZEOF(nodes));
	RT_ASSERT(verify_list(&list, nodes, RT_SIZEOF(nodes)) == RT_OK);
	dlist_free(&list, (op_unary_t)free_node);

	return RT_OK;
}

/**
 * Node adding to sorted list test case.
 */
RT_CASE(add_node_sorted)
{
	node_t nodes[] = {
			{.value = 4, .name = "four"},
			{.value = 1, .name = "one"},
			{.value = 6, .name = "six"},
			{.value = 2, .name = "two"},
			{.value = 3, .name = "three"},
		};
	node_t nodes_sorted[] = {
			{.value = 1, .name = "one"},
			{.value = 2, .name = "two"},
			{.value = 3, .name = "three"},
			{.value = 4, .name = "four"},
			{.value = 6, .name = "six"},
		};
	dlist_t list;
	dlist_init(&list);
	populate_list_sorted(&list, nodes, RT_SIZEOF(nodes));
	RT_ASSERT(verify_list(&list, nodes_sorted, RT_SIZEOF(nodes)) == RT_OK);
	dlist_free(&list, (op_unary_t)free_node);

	return RT_OK;
}

/**
 * Node finding test case.
 */
RT_CASE(find_node)
{
	node_t nodes[] = {
			{.value = 4, .name = "four"},
			{.value = 1, .name = "one"},
			{.value = 6, .name = "six"},
			{.value = 2, .name = "two"},
			{.value = 3, .name = "three"},
		};
	node_t *node, *tnode;
	dlist_t list;
	dlist_init(&list);
	populate_list(&list, nodes, RT_SIZEOF(nodes));
	RT_ASSERT(verify_find_failure(&list, 0) == RT_OK);
	RT_ASSERT(verify_find_failure(&list, 5) == RT_OK);
	RT_ASSERT(verify_find_failure(&list, 8) == RT_OK);

	tnode = &nodes[0];
	RT_ASSERT(verify_find_success(&list, tnode->value, tnode->value, tnode->name) == RT_OK);
	tnode = &nodes[2];
	RT_ASSERT(verify_find_success(&list, tnode->value, tnode->value, tnode->name) == RT_OK);
	tnode = &nodes[4];
	RT_ASSERT(verify_find_success(&list, tnode->value, tnode->value, tnode->name) == RT_OK);
	dlist_free(&list, (op_unary_t)free_node);

	return RT_OK;
}

/**
 * Node removing test case
 */
RT_CASE(remove_node)
{
	node_t nodes[] = {
			{.value = 4, .name = "four"},
			{.value = 6, .name = "six"},
			{.value = 2, .name = "two"},
			{.value = 3, .name = "three"},
		};
	node_t nodes1[] = {
			{.value = 4, .name = "four"},
			{.value = 2, .name = "two"},
			{.value = 3, .name = "three"},
		};
	node_t nodes2[] = {
			{.value = 4, .name = "four"},
			{.value = 2, .name = "two"},
		};
	node_t nodes3[] = {
			{.value = 2, .name = "two"},
		};
	node_t nodes4[] = {
		};
	node_t* node;
	dlist_t list;
	dlist_init(&list);
	populate_list(&list, nodes, RT_SIZEOF(nodes));

	node = dlist_find(&list, (void*)6, (op_binary_t)compare_node_value);
	RT_ASSERT_EX(node != 0, "id=%d", 6);
	dlist_remove(&list, node);
	free_node(node);
	RT_ASSERT(verify_list(&list, nodes1, RT_SIZEOF(nodes1)) == RT_OK);

	node = dlist_find(&list, (void*)3, (op_binary_t)compare_node_value);
	RT_ASSERT_EX(node != 0, "id=%d", 3);
	dlist_remove(&list, node);
	free_node(node);
	RT_ASSERT(verify_list(&list, nodes2, RT_SIZEOF(nodes2)) == RT_OK);

	node = dlist_find(&list, (void*)4, (op_binary_t)compare_node_value);
	RT_ASSERT_EX(node != 0, "id=%d", 4);
	dlist_remove(&list, node);
	free_node(node);
	RT_ASSERT(verify_list(&list, nodes3, RT_SIZEOF(nodes3)) == RT_OK);

	node = dlist_find(&list, (void*)2, (op_binary_t)compare_node_value);
	RT_ASSERT_EX(node != 0, "id=%d", 2);
	dlist_remove(&list, node);
	free_node(node);
	RT_ASSERT(verify_list(&list, nodes4, RT_SIZEOF(nodes4)) == RT_OK);

	dlist_free(&list, (op_unary_t)free_node);

	return RT_OK;
}

/* counter for unary iteration operations */
static int unary_counter;

/**
 * Value counting operation
 * @param node
 * @return
 */
static long count_values_unary(node_t* node)
{
	unary_counter += node->value;
	return 0;
}

/**
 * Unary operation iterator tests
 */
RT_CASE(iterate_unary)
{
	node_t nodes[] = {
			{.value = 4, .name = "four"},
			{.value = 1, .name = "one"},
			{.value = 2, .name = "two"},
			{.value = 3, .name = "three"},
		};
	dlist_t list;
	dlist_init(&list);
	populate_list(&list, nodes, RT_SIZEOF(nodes));

	unary_counter = 0;
	dlist_foreach(&list, (op_unary_t)count_values_unary);
	RT_ASSERT(unary_counter == 10);
	dlist_free(&list, (op_unary_t)free_node);

	return RT_OK;
}

/**
 * Value counting operation
 * @param node
 * @param counter
 * @return
 */
static long count_values_binary(node_t* node, int* counter)
{
	*counter += node->value;
	return 0;
}

/**
 * Iterate operation to remove nodes with value 2
 * @param node
 * @param list
 * @return
 */
static long remove_node_if_2(node_t* node, dlist_t* list)
{
	if (node->value == 2) {
		dlist_remove(list, node);
		free_node(node);
	}
}
/**
 * Binary operation iterator tests
 */
RT_CASE(iterate_binary)
{
	node_t nodes[] = {
			{.value = 4, .name = "four"},
			{.value = 1, .name = "one"},
			{.value = 2, .name = "two"},
			{.value = 3, .name = "three"},
		};
	node_t nodes_out[] = {
			{.value = 4, .name = "four"},
			{.value = 1, .name = "one"},
			{.value = 3, .name = "three"},
		};
	dlist_t list;
	dlist_init(&list);
	populate_list(&list, nodes, RT_SIZEOF(nodes));

	dlist_foreach2(&list, (op_binary_t)remove_node_if_2, (void*)&list);
	RT_ASSERT(verify_list(&list, nodes_out, RT_SIZEOF(nodes_out)) == RT_OK);

	dlist_free(&list, (op_unary_t)free_node);
	return RT_OK;
}

/**
 * Range checking function.
 * @param node
 * @return
 */
static long iterate_while_1(node_t* node)
{
	return  node->value <= 1;
}

/**
 * Range checking function.
 * @param node
 * @return
 */
static long iterate_while_3(node_t* node)
{
	return node->value <= 3;
}

/**
 * Unary range checking function.
 * @param node
 * @return
 */
static long iterate_while_4(node_t* node)
{
	return node->value <= 4;
}

/**
 * Unary operation range iterator tests
 */
RT_CASE(iterate_range_unary)
{
	node_t nodes[] = {
			{.value = 4, .name = "four"},
			{.value = 1, .name = "one"},
			{.value = 2, .name = "two"},
			{.value = 3, .name = "three"},
		};
	dlist_t list;
	dlist_init(&list);
	populate_list_sorted(&list, nodes, RT_SIZEOF(nodes));

	unary_counter = 0;
	dlist_node_t* node = dlist_first(&list);
	node = dlist_foreach_in(&list, node, (op_unary_t)iterate_while_1, (op_unary_t)count_values_unary);
	RT_ASSERT(unary_counter == 1);
	node = dlist_foreach_in(&list, node, (op_unary_t)iterate_while_3, (op_unary_t)count_values_unary);
	RT_ASSERT(unary_counter == 6);
	node = dlist_foreach_in(&list, node, (op_unary_t)iterate_while_3, (op_unary_t)count_values_unary);
	RT_ASSERT(unary_counter == 6);
	dlist_foreach_in(&list, node, (op_unary_t)iterate_while_4, (op_unary_t)count_values_unary);
	RT_ASSERT(unary_counter == 10);
	dlist_free(&list, (op_unary_t)free_node);

	return RT_OK;
}

/**
 * Binary range checking function.
 * @param node
 * @return
 */
static long iterate_while(node_t* node, int range)
{
	return node->value <= range;
}


/**
 * Binary operation range iterator tests
 */
RT_CASE(iterate_range_binary)
{
	node_t nodes[] = {
			{.value = 4, .name = "four"},
			{.value = 1, .name = "one"},
			{.value = 2, .name = "two"},
			{.value = 3, .name = "three"},
		};
	dlist_t list;
	dlist_init(&list);
	populate_list_sorted(&list, nodes, RT_SIZEOF(nodes));

	int counter = 0;
	dlist_node_t* node = dlist_first(&list);
	node = dlist_foreach2_in(&list, node, (op_binary_t)iterate_while, (void*)1, (op_binary_t)count_values_binary, (void*)&counter);
	RT_ASSERT(counter == 1);
	node = dlist_foreach2_in(&list, node, (op_binary_t)iterate_while, (void*)3, (op_binary_t)count_values_binary, (void*)&counter);
	RT_ASSERT(counter == 6);
	node = dlist_foreach2_in(&list, node, (op_binary_t)iterate_while, (void*)3, (op_binary_t)count_values_binary, (void*)&counter);
	RT_ASSERT(counter == 6);
	dlist_foreach2_in(&list, node, (op_binary_t)iterate_while, (void*)4, (op_binary_t)count_values_binary, (void*)&counter);
	RT_ASSERT(counter == 10);
	dlist_free(&list, (op_unary_t)free_node);

	return RT_OK;
}



/**
 *
 * @return
 */
int main()
{

	RT_START("dlist");

	RT_RUN_CASE(initialize);
	RT_RUN_CASE(initialize_array);
	RT_RUN_CASE(create_node);
	RT_RUN_CASE(add_node);
	RT_RUN_CASE(add_node_sorted);
	RT_RUN_CASE(find_node);
	RT_RUN_CASE(remove_node);
	RT_RUN_CASE(iterate_unary);
	RT_RUN_CASE(iterate_binary);
	RT_RUN_CASE(iterate_range_unary);
	RT_RUN_CASE(iterate_range_binary);

	return 0;
}
