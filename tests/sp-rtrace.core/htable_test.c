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
 * @file htable_test.c
 *
 * Test application for the hash table implementation.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "rtrace_testsuite.h"

#include "common/htable.h"
#include "common/utils.h"

RT_INIT();

#define HTABLE_SIZE 10

/**
 * The test node
 */
typedef struct {
	htable_node_t node;
	int id;
	char* text;
} node_t;


/**
 * Frees the test node.
 * @param node
 */
static void free_node(node_t* node)
{
	if (node->text) free(node->text);
	free(node);
}

static node_t* new_node(int id, const char* text)
{
	node_t* node = (node_t*)htable_create_node(sizeof(node_t));
	node->id = id;
	node->text = strdup_a(text);
	return node;
}

/**
 * Calculates hash value.
 * @param node
 * @return
 */
static long calc_hash(const node_t* node)
{
	const char* ptr = node->text;
	int hsumm = 0;
	hsumm = hsumm * 37 + (unsigned char)*ptr++;
	return hsumm % HTABLE_SIZE;
}

/**
 * Compares two test nodes.
 * @param node1
 * @param node2
 * @return
 */
static long compare_nodes(const node_t* node1, const node_t* node2)
{
	return strcmp(node1->text, node2->text);
}

/**
 * Verifies if the node matches specified data.
 * @param node
 * @param id
 * @param text
 * @return
 */
static int verify_node(const node_t* node, int id, const char* text)
{
	RT_ASSERT_EX(node->id == id, "(%d ? %d)", node->id, id);
	RT_ASSERT_EX(!strcmp(node->text, text), "(%s ? %s)", node->text, text);
	return RT_OK;
}

/**
 * Data structure used to check if node is stored in hash table.
 */
typedef struct {
	int id;
	const char *text;
	bool found;
} node_check_t;

/**
 * Iteration operator to check if the hash table bucket contains node
 * described in template.
 *
 * @param node
 * @param template
 * @return
 */
static long check_if_contains(node_t* node, node_check_t* template)
{
	if (node->id == template->id && !strcmp(node->text, template->text)) {
		template->found = true;
	}
}

/**
 * Verifies if the table already contains node with the specified data
 *
 * @param ht
 * @param id
 * @param text
 * @return
 */
static int verify_if_contains(htable_t* ht, int id, const char* text)
{
	int i;
	node_check_t template = {.id = id, .text = text, .found = false};
	for (i = 0; i < ht->size && !template.found; i++) {
		dlist_foreach2(&ht->buckets[i], (op_binary_t)check_if_contains, (void*)&template);
	}
	return template.found ? RT_OK : RT_FAIL;
}


/**
 * Verifies if the table does not contain node with the specified data
 *
 * @param ht
 * @param id
 * @param text
 * @return
 */
static int verify_if_not_contains(htable_t* ht, int id, const char* text)
{
	int i;
	node_check_t template = {.id = id, .text = text, .found = false};
	for (i = 0; i < ht->size && !template.found; i++) {
		dlist_foreach2(&ht->buckets[i], (op_binary_t)check_if_contains, (void*)&template);
	}
	return template.found ? RT_FAIL : RT_OK;
}

/**
 * Initialization test case.
 */
RT_CASE(initialize)
{
	htable_t table;
	htable_init(&table, HTABLE_SIZE, (op_unary_t)calc_hash, (op_binary_t)compare_nodes);
	RT_ASSERT(table.buckets != NULL);
	RT_ASSERT(table.size = HTABLE_SIZE);
	RT_ASSERT(table.do_compare == (op_binary_t)compare_nodes);
	RT_ASSERT(table.do_calc_hash = (op_unary_t)calc_hash);
	int i;
	for (i = 0; i < HTABLE_SIZE; i++) {
		RT_ASSERT(table.buckets[i].tail == NULL);
		RT_ASSERT(table.buckets[i].head == NULL);
	}
	htable_free(&table, (op_unary_t)free_node);
	return RT_OK;
}

/**
 * Node creation test case.
 */
RT_CASE(create_node)
{
	node_t* node = (node_t*)htable_create_node(sizeof(node_t));
	RT_ASSERT(node != NULL);
	RT_ASSERT(node->node.bucket == NULL);
	RT_ASSERT(node->node.node.next == NULL);
	RT_ASSERT(node->node.node.prev == NULL);
	free_node(node);
	return RT_OK;
}

/**
 * Node storing test case.
 */
RT_CASE(store_node)
{
	node_t template[] = {
			{.id = 1, .text = "12345"},
			{.id = 2, .text = "12345"},
		};
	htable_t table;
	htable_init(&table, HTABLE_SIZE, (op_unary_t)calc_hash, (op_binary_t)compare_nodes);

	node_t* node = htable_store(&table, new_node(template[0].id, template[0].text));
	RT_ASSERT(node == NULL);
	RT_ASSERT_EX(verify_if_contains(&table, template[0].id, template[0].text) == RT_OK, "(%d:%s)", template[0].id, template[0].text);

	node = htable_store(&table, new_node(template[1].id, template[1].text));
	RT_ASSERT(node != NULL);
	RT_ASSERT(verify_node(node, template[0].id, template[0].text) == RT_OK);
	free_node(node);
	RT_ASSERT_EX(verify_if_not_contains(&table, template[0].id, template[0].text) == RT_OK, "(%d:%s)", template[0].id, template[0].text);
	RT_ASSERT_EX(verify_if_contains(&table, template[1].id, template[1].text) == RT_OK, "(%d:%s)", template[1].id, template[1].text);

	int i;
	for (i = 0; i < HTABLE_SIZE * 10; i++) {
		char buffer[10];
		sprintf(buffer, "%05d", i);
		node_t template = {.id = i, .text = buffer};

		node = htable_store(&table, new_node(template.id, template.text));
		RT_ASSERT(node == NULL);
		RT_ASSERT_EX(verify_if_contains(&table, template.id, template.text) == RT_OK, "(%d:%s)", template.id, template.text);
	}

	htable_free(&table, (op_unary_t)free_node);
	return RT_OK;
}


/**
 * Node finding test case.
 */
RT_CASE(find_node)
{
	htable_t table;
	htable_init(&table, HTABLE_SIZE, (op_unary_t)calc_hash, (op_binary_t)compare_nodes);

	int i;
	for (i = 0; i < HTABLE_SIZE * 10; i++) {
		char buffer[10];
		sprintf(buffer, "%05d", i);
		node_t template = {.id = i, .text = buffer};
		htable_store(&table, new_node(template.id, template.text));
	}
	node_t template_in[] = {
			{.id = 0, .text = "00000"},
			{.id = 10, .text = "00010"},
			{.id = 16, .text = "00016"},
			{.id = 27, .text = "00027"},
	};
	node_t template_out[] = {
			{.id = 0, .text = "11111"},
			{.id = 1, .text = "22222"},
			{.id = 2, .text = "33333"},
			{.id = 3, .text = "44444"},
	};

	for (i = 0; i < RT_SIZEOF(template_in); i++) {
		node_t* node = htable_find(&table, &template_in[i]);
		RT_ASSERT_EX(node != NULL, "text=%s", template_in[i].text);
		RT_ASSERT_EX(verify_node(node, template_in[i].id, template_in[i].text) == RT_OK, "text=%s", template_in[i].text);
	}
	for (i = 0; i < RT_SIZEOF(template_out); i++) {
		node_t* node = htable_find(&table, &template_out[i]);
		RT_ASSERT_EX(node == NULL, "text=%s", template_in[i].text);
	}

	htable_free(&table, (op_unary_t)free_node);
	return RT_OK;
}

/**
 * Node removal test case.
 */
RT_CASE(remove_node)
{
	htable_t table;
	htable_init(&table, HTABLE_SIZE, (op_unary_t)calc_hash, (op_binary_t)compare_nodes);

	int i, j, k;
	for (i = 0; i < HTABLE_SIZE * 10; i++) {
		char buffer[10];
		sprintf(buffer, "%05d", i);
		node_t template = {.id = i, .text = buffer};
		htable_store(&table, new_node(template.id, template.text));
	}

	node_t template_in[] = {
			{.id = 1, .text = "00001"},
			{.id = 11, .text = "00011"},
			{.id = 15, .text = "00015"},
			{.id = 33, .text = "00033"},
	};

	for (i = 0; i < RT_SIZEOF(template_in); i++) {
		node_t* node = htable_find(&table, &template_in[i]);
		RT_ASSERT(node != NULL);
		htable_remove_node(node);
		RT_ASSERT_EX(verify_if_not_contains(&table, template_in[0].id, template_in[0].text) == RT_OK, "(%d:%s)", template_in[0].id, template_in[0].text);
		free_node(node);

		for (j = 0; j < HTABLE_SIZE; j++) {
			bool pass = false;
			for (k = 0; k <= i; k++) {
				if (template_in[k].id == j) {
					pass = true;
					break;
				}
			}
			if (!pass) {
				char buffer[10];
				sprintf(buffer, "%05d", j);
				node_t template = {.id = j, .text = buffer};
				RT_ASSERT_EX(verify_if_contains(&table, template.id, template.text) == RT_OK, "(%d:%s)", template.id, template.text);
			}
		}
	}

	htable_free(&table, (op_unary_t)free_node);
	return RT_OK;
}

/**
 * Unary operation iteration test counter
 */
static int unary_counter;

/**
 * Node id summing function for unary iteration tests.
 * @param node
 * @return
 */
static long count_nodes_unary(node_t* node)
{
	unary_counter += node->id;
}

/**
 * Unary operation iterator test case.
 */
RT_CASE(iterate_unary)
{
	htable_t table;
	htable_init(&table, HTABLE_SIZE, (op_unary_t)calc_hash, (op_binary_t)compare_nodes);

	int i, j, k;
	for (i = 0; i < HTABLE_SIZE * 10; i++) {
		char buffer[10];
		sprintf(buffer, "%05d", i);
		node_t template = {.id = i, .text = buffer};
		htable_store(&table, new_node(template.id, template.text));
	}

	unary_counter = 0;
	htable_foreach(&table, (op_unary_t)count_nodes_unary);
	RT_ASSERT_EX(unary_counter == 4950, "unary_counter=%d", unary_counter);

	htable_free(&table, (op_unary_t)free_node);
	return RT_OK;
}

/**
 * Node id summing function for binary iteration tests.
 * @param node
 * @return
 */
static long count_nodes_binary(node_t* node, int* counter)
{
	*counter += node->id;
}

/**
 * Unary operation iterator test case.
 */
RT_CASE(iterate_binary)
{
	htable_t table;
	htable_init(&table, HTABLE_SIZE, (op_unary_t)calc_hash, (op_binary_t)compare_nodes);

	int i, j, k;
	for (i = 0; i < HTABLE_SIZE * 10; i++) {
		char buffer[10];
		sprintf(buffer, "%05d", i);
		node_t template = {.id = i, .text = buffer};
		htable_store(&table, new_node(template.id, template.text));
	}

	int counter = 0;
	htable_foreach2(&table, (op_binary_t)count_nodes_binary, (void*)&counter);
	RT_ASSERT_EX(counter == 4950, "counter=%d", counter);

	htable_free(&table, (op_unary_t)free_node);
	return RT_OK;
}

/**
 *
 * @return
 */
int main()
{
	RT_START("htable");
	RT_RUN_CASE(initialize);
	RT_RUN_CASE(create_node);
	RT_RUN_CASE(store_node);
	RT_RUN_CASE(find_node);
	RT_RUN_CASE(remove_node);
	RT_RUN_CASE(iterate_unary);
	RT_RUN_CASE(iterate_binary);

	return 0;

}

