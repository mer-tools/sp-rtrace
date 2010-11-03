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

#include "filter.h"

#include "common/sp_rtrace_proto.h"
#include "common/utils.h"
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
void free_fres_rec(fres_t* res)
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
	if (res1->call->res_type == res2->call->res_type) {
		return res1->call->res_id - res2->call->res_id;
	}
	if (!res1->call->res_type) {
		fprintf(stderr, "WARNING: resource type is not set for record #%d\n", res1->call->index);
		return -1;
	}
	if (!res2->call->res_type) {
		fprintf(stderr, "WARNING: resource type is not set for record #%d\n", res1->call->index);
		return 1;
	}
	return res1->call->res_type->id - res2->call->res_type->id;
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
	unsigned long value = (unsigned long)res->call->res_id;
	while (value) {
		hash ^= value & ((1 << 16) - 1);
		value >>= 3;
	}
	if (res->call->res_type) {
		value = res->call->res_type->id;
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

    if (call->type == SP_RTRACE_FTYPE_ALLOC) {
    	if (res) {
    		res->ref_count++;
			rd_fcall_remove(index->rd, call);
    	}
    	else {
    		/* create resource index record */
			fres_t* new_res = (fres_t*)calloc_a(1, sizeof(fres_t));
			new_res->call = call;
			new_res->ref_count = 1;
			/* store the created record into resource index table */
			htable_store(&index->table, new_res);
    	}
    }
    else if (call->type == SP_RTRACE_FTYPE_FREE) {
    	/* create resource template for htable_find function. As the
    	 * compare function of resource hash table uses only call->res
    	 * id value, setting the .call field is enough for lookups */
        if (res) {
        	res->ref_count--;
        	if (res->ref_count == 0) {
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
	if ( (postproc_options.filter_context && !(call->context & postproc_options.filter_context)) ||
			(!postproc_options.filter_context && call->context) ) {
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
	if (call->res_type && !( (1 << (call->res_type->id - 1)) & postproc_options.filter_resource)) {
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
	if (call->type == SP_RTRACE_FTYPE_ALLOC) {
		if (call->res_id < hinfo->lowest_block) {
			hinfo->lowest_block = call->res_id;
		}
		if (call->res_id > hinfo->highest_block) {
			hinfo->highest_block = call->res_id;
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
	if (! (context->id & postproc_options.filter_context) ) {
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
	if (! ( (1 << (resource->id - 1)) & postproc_options.filter_resource) ) {
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
static int trim_backtrace(rd_ftrace_t* trace, int backtrace_depth)
{
	if (trace->nframes > backtrace_depth) {
		trace->nframes = backtrace_depth;
	}
	return 0;
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
		fprintf(stderr, "ERROR: failed to create resource indexing table\n");
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
		res->id = 1;
	}
}

void filter_find_lowhigh_blocks(rd_t* rd)
{
	rd->hinfo->lowest_block = (pointer_t)~0L;
	dlist_foreach2(&rd->calls, (op_binary_t)fcall_find_lowhigh_blocks, (void*)rd->hinfo);
}


long filter_sum_leaks(rd_fcall_t* call, leak_data_t* leaks)
{
	if (call->type == SP_RTRACE_FTYPE_ALLOC) {
		/* Resource type 0 is used when only when one resource type is present, to
		 * hide the resource types in call reports. In reality the resource type
		 * is 1	 */
		leak_data_t* leak = &leaks[call->res_type ? call->res_type->id - 1 : 0];
		leak->count++;
		leak->total_size += call->res_size;
	}
	return 0;
}


void filter_trim_backtraces(rd_t* rd)
{
	/* update backtrace depth in the process info packet */
	rd->pinfo->backtrace_depth = postproc_options.backtrace_depth;

	/* trim the backtraces. Note that only backtrace size is changed, the allocated memory
	 * is not reallocated */
	htable_foreach2(&rd->ftraces, (op_binary_t)trim_backtrace, (void*)rd->pinfo->backtrace_depth);
}
