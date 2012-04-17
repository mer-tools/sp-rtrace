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

#define _GNU_SOURCE

#include <stdbool.h>
#include <search.h>
#include <stdlib.h>
#include <stdio.h>

#include "filter.h"

#include "common/sp_rtrace_proto.h"
#include "common/resolve_utils.h"
#include "common/utils.h"
#include "common/msg.h"
#include "sp_rtrace_postproc.h"

#define HASH_SIZE      (1 << 16)


/**
 * Resource index data.
 *
 * The resource index data contains reference to the associated
 * function call. The call->res_id field is used as indexing value.
 */
typedef struct fres_t {
	/* hash table support */
	htable_node_t node;
	/* the associated function call */
	rd_fcall_t* call;
	/* resource reference counter */
	int ref_count;
} fres_t;


/**
 * Resource index data structure.
 *
 * Used to wrap the data required by check_freed_call function
 * into single argument.
 */
typedef struct fres_index_t {
	/* the rtrace data */
	rd_t* rd;
	/* the resource->function calls index table */
	htable_t table;
} fres_index_t;

/**
 * Frees resource index data.
 *
 * @param[in] res  the data to free.
 */
static void free_fres_rec(fres_t* res)
{
	free(res);
}


/**
 * Compare two resources.
 *
 * The resources are compared by comparing their identifiers, which
 * essentially are the record pointers.
 * @param[in] res1  the first backtrace.
 * @param[in] res2  the second backtrace.
 * @return          <0 - the first backtrace is 'less' than second.
 *                   0 - the backtraces are equal.
 *                  >0 - the first backtrace is 'greater' than second.
 */
static long res_compare(const fres_t* res1, const fres_t* res2)
{
	if (res1->call->data.res_type == res2->call->data.res_type) {
		return res1->call->data.res_id - res2->call->data.res_id;
	}
	if (!res1->call->data.res_type) {
		msg_warning("resource type is not set for record #%d\n", res1->call->data.index);
		return -1;
	}
	if (!res2->call->data.res_type) {
		msg_warning("resource type is not set for record #%d\n", res1->call->data.index);
		return 1;
	}
	rd_resource_t* res_type1 = res1->call->data.res_type;
	rd_resource_t* res_type2 = res2->call->data.res_type;
	return res_type1->data.id - res_type2->data.id;
}

/**
 * Calculates hash value for the resource.
 *
 * TODO: find better hash function.
 * @param[in] res   the resource index data.
 * @return          the hash value.
 */
static long res_hash(const fres_t* res)
{
	unsigned long hash = 0;
	unsigned long value = (unsigned long)res->call->data.res_id;
	while (value) {
		hash ^= value & ((1 << 16) - 1);
		value >>= 3;
	}
	if (res->call->data.res_type) {
		rd_resource_t* res_type = res->call->data.res_type;
		value = res_type->data.id;
		while (value) {
			hash ^= value & ((1 << 16) - 1);
			value >>= 3;
		}
	}
	return hash;
}



/**
 * Checks and removes calls if necessary.
 *
 * This function checks if a resource freed by deallocation
 * function has been previously allocated by an allocation function.
 * If such function is found, it is removed from function call list.
 * The deallocation functions are removed always.
 * To speed up the allocation function lookup by resource id
 * hash table based index is used (fres_index_t::index).
 * @param[in] call  the function call record to check.
 * @param[in] data  the indexing data (see fres_index_t structure)
 * @return
 */
static long fcall_remove_freed(rd_fcall_t* call, void* data)
{
	fres_index_t* index = (fres_index_t*)data;
	fres_t find_res = {.call = call};
    fres_t* res = htable_find(&index->table, &find_res);
    rd_resource_t* res_type = call->data.res_type;

    if (call->data.type == SP_RTRACE_FTYPE_ALLOC) {
    	if (res && (res_type->data.flags & SP_RTRACE_RESOURCE_REFCOUNT)) {
    		res->ref_count++;
			rd_fcall_remove(index->rd, call);
    	}
    	else {
    		/* create resource index record */
			fres_t* new_res = (fres_t*)calloc_a(1, sizeof(fres_t));
			new_res->call = call;
			new_res->ref_count = 1;
			/* store the created record into resource index table */
			fres_t* old_res = htable_store(&index->table, new_res);
			if (old_res) free_fres_rec(old_res);
    	}
    }
    else if (call->data.type == SP_RTRACE_FTYPE_FREE) {
    	/* create resource template for htable_find function. As the
    	 * compare function of resource hash table uses only call->res
    	 * id value, setting the .call field is enough for lookups */
        if (res) {
        	res->ref_count--;
        	if (res->ref_count == 0 || !(res_type->data.flags & SP_RTRACE_RESOURCE_REFCOUNT)) {
				/* The resource allocation record found. Remove the record
				 * from function call list and free it. Also remove and
				 * free the resource index record */
				htable_remove_node((htable_node_t*)res);
				rd_fcall_remove(index->rd, res->call);
				free_fres_rec(res);
        	}
        }
        /* deallocation call record is always removed */
        rd_fcall_remove(index->rd, call);
    }
    return 0;
}


/**
 * Removes function call record if its context doesn't match filtering mask.
 *
 * @param[in] call   the function call record to check.
 * @param[in] data   the rtrace data.
 * @return
 */
static void fcall_filter_context(rd_fcall_t* call, rd_t* rd)
{
	if ( (postproc_options.filter_context && !(call->data.context & postproc_options.filter_context)) ||
			(!postproc_options.filter_context && call->data.context) ) {
		rd_fcall_remove(rd, call);
	}
}


/**
 * Removes function call record if its module id doesn't match filtering mask.
 *
 * @param[in] call   the function call record to check.
 * @param[in] data   the rtrace data.
 * @return
 */
static void fcall_filter_resource(rd_fcall_t* call, rd_t* rd)
{
	rd_resource_t* res = call->data.res_type;
	if (res && !( (1 << (res->data.id - 1)) & postproc_options.filter_resource)) {
		rd_fcall_remove(rd, call);
	}
}

/**
 * Find the lowest and highest allocation blocks.
 *
 * @param[in] call   the function call record.
 * @param[in] hinfo  the heap information data.
 * @return
 */
static void fcall_find_lowhigh_blocks(rd_fcall_t* call, rd_hinfo_t* hinfo)
{
	if (call->data.type == SP_RTRACE_FTYPE_ALLOC) {
		if (call->data.res_id < hinfo->lowest_block) {
			hinfo->lowest_block = call->data.res_id;
		}
		if (call->data.res_id > hinfo->highest_block) {
			hinfo->highest_block = call->data.res_id;
		}
	}
}

/**
 * Removes context records not matching the specified context filter.
 *
 * @param[in] context  the context to check.
 * @param[in] list     the context list.
 */
static void context_filter_mask(rd_context_t* context, dlist_t* list)
{
	if (! (context->data.id & postproc_options.filter_context) ) {
		dlist_remove(list, (void*)context);
	}
}

/**
 * Removes resource type records not matching the specified resource filter.
 *
 * @param[in] resource  the resource to check.
 * @param[in] list      the resource list.
 */
static void resource_filter_mask(rd_resource_t* resource, dlist_t* list)
{
	if (! ( (1 << (resource->data.id - 1)) & postproc_options.filter_resource) ) {
		dlist_remove(list, (void*)resource);
	}
}

/**
 * Trims backtrace size to be less or equal to the maximum backtrace depth.
 * 
 * This function simply updates backtrace 'size' property without raellocating
 * the real data. 
 * @param trace 
 * @param fmt
 * @return
 */
static int trim_backtrace(rd_ftrace_t* trace, unsigned int backtrace_depth)
{
	if (trace->data.nframes > backtrace_depth) {
		trace->data.nframes = backtrace_depth;
	}
	return 0;
}


/**
 * Compares two event indexes.
 *
 * @param[in] index1   the first index.
 * @param[in] index2   the second index.
 * @return             -1 - index1 < index2
 *                     =0 - index1 = index2
 *                     +1 - index1 > index2
 */
static int filter_compare_event_index(const void* index1, const void* index2)
{
	return (long)index1 - (long)index2;
}

/**
 * Loads event index data from a file.
 *
 * The input file contains one event index per line. This function reads the
 * file and stores the index data into binary tree.
 * @param[in] filename   the event index file name.
 * @return                a binary tree containing index data.
 */
static void* filter_load_index_data(const char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {
		msg_error("failed to open event index file: %s (%s)\n", filename, strerror(errno));
		exit (-1);
	}
	char line[128];
	void* root = NULL;
	while (fgets(line, sizeof(line), fp)) {
		unsigned long index;
		if (sscanf(line, "%li", &index) == 1) {
			tsearch((void*)index, &root, filter_compare_event_index);
		}
	}
	fclose(fp);
	return root;
}

/**
 * Helper data structure containing necessary data for index filter.
 */
typedef struct index_filter_t {
	/* the rtrace data */
	rd_t* rd;
	/* true if include filter is applied, false if exclude */
	bool include;
	/* the include/exclude index map */
	void* index_map;
} index_filter_t;


/**
 * The event filtering function.
 *
 * This function performs call index lookup in the index tree and removes
 * call if:
 *   1) exclude rule is set and index was found
 *   2) include rule is set and index was not found
 * @param[in] call    the call to check.
 * @param[in] filter  the index filtering data.
 */
static void fcall_filter_index(rd_fcall_t* call, index_filter_t* filter)
{
	unsigned long index = call->data.index;
	bool found = tfind((void*)index, &filter->index_map, filter_compare_event_index);
	if ((found && !filter->include) || (!found && filter->include)) {
		rd_fcall_remove(filter->rd, call);
	}
}

/**
 * Empty function to pass to tdestroy.
 * @param
 */
static void free_index(void* data __attribute__((unused)))
{
}

/*
 * Public API
 */


void filter_leaks(rd_t* rd)
{
	/* indexing structure to wrap needed data into single argument */
	fres_index_t index = {.rd = rd};
	/* create resource indexing hash table */
	if (htable_init(&index.table, HASH_SIZE, (op_unary_t)res_hash, (op_binary_t)res_compare) != 0) {
		msg_error("failed to create resource indexing table\n");
		exit (-1);
	}

	dlist_foreach2(&rd->calls, (op_binary_t)fcall_remove_freed, (void*)&index);

	htable_free(&index.table, (op_unary_t)free_fres_rec);
}

void filter_context(rd_t* rd)
{
	dlist_foreach2(&rd->contexts, (op_binary_t)context_filter_mask, (void*)&rd->contexts);
	dlist_foreach2(&rd->calls, (op_binary_t)fcall_filter_context, (void*)rd);
}

void filter_resource(rd_t* rd)
{
	dlist_foreach2(&rd->resources, (op_binary_t)resource_filter_mask, (void*)&rd->resources);
	dlist_foreach2(&rd->calls, (op_binary_t)fcall_filter_resource, (void*)rd);
}

void filter_update_resource_visibility(rd_t* rd)
{
	if (dlist_first(&rd->resources) && dlist_first(&rd->resources) == dlist_last(&rd->resources)) {
		/* if only one resource is present, reset its index and hide it */
		rd_resource_t* res = (rd_resource_t*)dlist_first(&rd->resources);
		res->hide = true;
		res->data.id = 1;
	}
}

void filter_find_lowhigh_blocks(rd_t* rd)
{
	rd->hinfo->lowest_block = (pointer_t)~0L;
	dlist_foreach2(&rd->calls, (op_binary_t)fcall_find_lowhigh_blocks, (void*)rd->hinfo);
}


long filter_sum_leaks(rd_fcall_t* call, leak_data_t* leaks)
{
	if (call->data.type == SP_RTRACE_FTYPE_ALLOC) {
		/* Resource type 0 is used when only when one resource type is present, to
		 * hide the resource types in call reports. In reality the resource type
		 * is 1	 */
		rd_resource_t* res = call->data.res_type;
		leak_data_t* leak = &leaks[res ? res->data.id - 1 : 0];
		leak->count++;
		leak->total_size += call->data.res_size;
	}
	return 0;
}


void filter_trim_backtraces(rd_t* rd)
{
	/* update backtrace depth in the process info packet */
	rd->pinfo->backtrace_depth = postproc_options.backtrace_depth;

	/* trim the backtraces. Note that only backtrace size is changed, the allocated memory
	 * is not reallocated */
	unsigned long bt_depth = rd->pinfo->backtrace_depth;
	htable_foreach2(&rd->ftraces, (op_binary_t)trim_backtrace, (void*)bt_depth);
}


void filter_include(rd_t* rd)
{
	index_filter_t filter = {
			.rd = rd,
			.include = true,
			.index_map = filter_load_index_data(postproc_options.include_file),
	};

	dlist_foreach2(&rd->calls, (op_binary_t)fcall_filter_index, (void*)&filter);
	if (filter.index_map) tdestroy(filter.index_map, free_index);
}


void filter_exclude(rd_t* rd)
{
	index_filter_t filter = {
			.rd = rd,
			.include = false,
			.index_map = filter_load_index_data(postproc_options.exclude_file),
	};

	dlist_foreach2(&rd->calls, (op_binary_t)fcall_filter_index, (void*)&filter);
	if (filter.index_map) tdestroy(filter.index_map, free_index);
}

/*
 * Code address range filtering support
 */


/**/
typedef struct {
	rd_t* rd;
	unsigned long start;
	unsigned long size;
} call_address_filter_t;


/**
 * Calculates the real code address based on target name and specified address range.
 * @param mmap
 * @param filter
 */
static long mmap_lookup_filter_range_target(rd_mmap_t* mmap, call_address_filter_t* filter)
{
	if (strstr(mmap->data.module, postproc_options.filter_range_target)) {
		filter->start = postproc_options.filter_range_start;
		filter->size = postproc_options.filter_range_size;
		if (!rs_mmap_is_absolute(mmap->data.module)) {
			filter->start += mmap->data.from;
		}
	}
	return 0;
}

/**
 * Checks if the code address range was found.
 * @param mmap
 * @param filter
 */
static long mmap_lookup_filter_range_target_found(rd_mmap_t* mmap, call_address_filter_t* filter)
{
	return !filter->start;
}

/**
 * Removes allocation events with backtraces not containing any addresses in the
 * specified range.
 * @param call
 * @param filter
 */
static void fcall_filter_range(rd_fcall_t* call, call_address_filter_t* filter)
{
	unsigned int i;
	for (i = 0; i < call->trace->data.nframes; i++) {
		pointer_t address = call->trace->data.frames[i];
		if (address >= filter->start && address < filter->start + filter->size) {
			return;
		}
	}
	rd_fcall_remove(filter->rd, call);
}


void filter_call_address_range(rd_t* rd)
{
	call_address_filter_t filter = {
		.rd = rd,
		.start = 0,
		.size = 0,
	};

	dlist_foreach2_in(&rd->mmaps, dlist_first(&rd->mmaps), (op_binary_t)mmap_lookup_filter_range_target_found,
			(void*)&filter, (op_binary_t)mmap_lookup_filter_range_target, (void*)&filter);
	if (!filter.start) {
		msg_warning("failed to find the specified call address range target: %s\n", postproc_options.filter_range_target);
		free(postproc_options.filter_range_target);
		postproc_options.filter_range_target = NULL;
		return;
	}

	dlist_foreach2(&rd->calls, (op_binary_t)fcall_filter_range, (void*)&filter);
}
