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
#include <time.h>

#include "rtrace_data.h"
#include "utils.h"

/*
 * Resource trace data freeing function implementations.
 */
void rd_hashake_free(rd_hshake_t* hs)
{
	if (hs->arch) free(hs->arch);
	free(hs);
}

void rd_pinfo_free(rd_pinfo_t* pinfo)
{
	if (pinfo->name) free(pinfo->name);
	free(pinfo);
}

void rd_minfo_free(rd_minfo_t* minfo)
{
	if (minfo->name) free(minfo->name);
	free(minfo);
}

void rd_mmap_free(rd_mmap_t* mmap)
{
	if (mmap->module) free(mmap->module);
	free(mmap);
}

void rd_context_free(rd_context_t* context)
{
	if (context->name) free(context->name);
	free(context);
}


void rd_resource_free(rd_resource_t* resource)
{
	if (resource->type) free(resource->type);
	if (resource->desc) free(resource->desc);
	free(resource);
}

void rd_fcall_free(rd_fcall_t* call)
{
	if (call->name) free(call->name);
	free(call);
}

void rd_ftrace_free(rd_ftrace_t* trace)
{
	if (trace->frames) free(trace->frames);
	if (trace->resolved_names) {
		unsigned int i;
		for (i = 0; i < trace->nframes; i++) {
			if (trace->resolved_names[i]) {
				free(trace->resolved_names[i]);
			}
		}
		free(trace->resolved_names);
	}
	/* Free the function call references, but don't touch
	 * the function calls themselves. */
	dlist_free(&trace->calls, (op_unary_t)free);
	free(trace);
}

void rd_fargs_free(rd_fargs_t* args)
{
	if (args->args) {
		char** ptr = args->args;
		while (*ptr) {
			free(*ptr++);
		}
		free(args->args);
	}
	free(args);
}

void rd_comment_free(rd_comment_t* comment)
{
	if (comment->text) free(comment->text);
	free(comment);
}

void rd_hinfo_free(rd_hinfo_t* hinfo)
{
	free(hinfo);
}

/*
 * Utility functions used internally by the records
 */

/* the hash table size for backtrace and resource indices */
#define HASH_SIZE      (1 << 16)


/*
 * Backtrace indexing support
 */

/**
 * Compare two backtraces.
 *
 * @param[in] bt1   the first backtrace.
 * @param[in] bt2   the second backtrace.
 * @return          <0 - the first backtrace is 'less' than second.
 *                   0 - the backtraces are equal.
 *                  >0 - the first backtrace is 'greater' than second.
 */
static long bt_compare(const rd_ftrace_t* bt1, const rd_ftrace_t* bt2)
{
	if (bt1->nframes == bt2->nframes) {
		return memcmp(bt1->frames, bt2->frames, bt1->nframes * sizeof(bt1->frames[0]));
	}
	return bt1->nframes - bt2->nframes;
}

/**
 * Calculates hash value for the backtrace.
 *
 * TODO: find better hash function.
 * @param[in] bt     the backtrace.
 * @return           the hash value.
 */
static long bt_hash(rd_ftrace_t* bt)
{
	unsigned long hash = 0, i;
	for (i = 0; i < bt->nframes; i++) {
		unsigned long value = (unsigned long)bt->frames[i];
		while (value) {
			hash ^= value & ((1 << 16) - 1);
			value >>= 3;
		}
	}
	return hash;
}

/*
 * Public API implementation.
 */


rd_t* rd_create()
{
	/* allocate the structure itself */
	rd_t* rd = malloc_a(sizeof(rd_t));
	/* initialize data containers */
	dlist_init(&rd->calls);
	dlist_init(&rd->contexts);
	dlist_init(&rd->minfo);
	dlist_init(&rd->comments);
	dlist_init(&rd->resources);
	htable_init(&rd->ftraces, HASH_SIZE, (op_unary_t)bt_hash, (op_binary_t)bt_compare);
	dlist_init(&rd->mmaps);
	/* initialize single records */
	rd->hshake = NULL;
	rd->pinfo = NULL;
	rd->hinfo = NULL;

	return rd;
}


void rd_free(rd_t* data)
{
	/* frees the containers itself */
	htable_free(&data->ftraces, (op_unary_t)rd_ftrace_free);
	dlist_free(&data->calls, (op_unary_t)rd_fcall_free);
	dlist_free(&data->contexts, (op_unary_t)rd_context_free);
	dlist_free(&data->minfo, (op_unary_t)rd_minfo_free);
	dlist_free(&data->comments, (op_unary_t)rd_comment_free);
	dlist_free(&data->mmaps, (op_unary_t)rd_mmap_free);
	dlist_free(&data->resources, (op_unary_t)rd_resource_free);

	/* free single records */
	if (data->hshake) rd_hashake_free(data->hshake);
	if (data->pinfo) rd_pinfo_free(data->pinfo);
	if (data->hinfo) rd_hinfo_free(data->hinfo);

	free(data);
}

/**
 * Associate call reference with backtrace record.
 *
 * @param call
 * @param trace
 * @return
 */
static long store_fcall_trace(ref_node_t* call, rd_ftrace_t* trace)
{
	((rd_fcall_t*)call->ref)->trace = trace;
	((rd_fcall_t*)call->ref)->ref = call;
	trace->ref_count++;
	dlist_add(&trace->calls, call);
	return 0;
}

/*
 * Utility functions
 */


/**
 * Compares reference node with a pointer.
 *
 * @param[in] node   the reference node.
 * @param[in] call   the pointer to look for.
 * @return           0 if the reference node refers to the specified pointer.
 */
static long compare_ref(ref_node_t* node, void* call)
{
	return node->ref - call;
}

void rd_fcall_remove(rd_t* rd, rd_fcall_t* call)
{
	dlist_remove(&rd->calls, call);
	if (call->trace) {
		if (call->ref) {
			dlist_remove(&call->trace->calls, call->ref);
			free(call->ref);
		}
		call->trace->ref_count--;
		if (!call->trace->ref_count) {
			dlist_remove(call->trace->node.bucket, call->trace);
			rd_ftrace_free(call->trace);
		}
	}
	rd_fcall_free(call);
}

void rd_fcall_set_ftrace(rd_t* rd, rd_fcall_t* call, rd_ftrace_t* trace)
{
    /* Check if a matching backtrace has already been stored into
     * backtrace table. */
    rd_ftrace_t* xtrace = (rd_ftrace_t*)htable_find(&rd->ftraces, (htable_node_t*)trace);
    if (!xtrace) {
        /* unregistered backtrace. store it */
        htable_store(&rd->ftraces, (htable_node_t*)trace);
    }
    else {
        /* registered backtrace. Use the one from backtrace table and
         * free the created backtrace. */
        rd_ftrace_free(trace);
        trace = xtrace;
    }
    trace->ref_count++;
    call->trace = trace;
    ref_node_t* node =(ref_node_t*)dlist_create_node(sizeof(ref_node_t));
    node->ref = call;
    call->ref = node;
    dlist_add(&trace->calls, node);
}


void rd_fcalls_set_ftrace(rd_t* rd, dlist_t* calls, rd_ftrace_t* trace)
{
    /* Check if a matching backtrace has already been stored into
     * backtrace table. */
    rd_ftrace_t* xtrace = (rd_ftrace_t*)htable_find(&rd->ftraces, (htable_node_t*)trace);
    if (!xtrace) {
        /* unregistered backtrace. store it */
        htable_store(&rd->ftraces, (htable_node_t*)trace);
    }
    else {
        /* registered backtrace. Use the one from backtrace table and
         * free the created backtrace. */
        rd_ftrace_free(trace);
        trace = xtrace;
    }
    dlist_foreach2(calls, (op_binary_t)store_fcall_trace, (void*)trace);
}

