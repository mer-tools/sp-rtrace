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
#ifndef FILTER_H
#define FILTER_H

/**
 * @file postproc.h
 *
 * Contains post-processing logic implementation for sp-rtrace-postproc.
 */

#include <stdlib.h>

#include "common/rtrace_data.h"



/**
 * The leaks summary data structure, used
 * to calculate total leak count and size.
 */
typedef struct {
	int count;
	size_t total_size;
} leak_data_t;


/**
 * Filters leaked resources by removing allocation and deallocation
 * function call records for the freed resources.
 *
 * This function is used to process the --remove-freed command line option.
 * @param[in] rd   the resource trace data storage.
 * @return
 */
void filter_leaks(rd_t* rd);


/**
 * Filters allocations with contexts matching the specified context filter.
 *
 * @param[in] rd   the resource trace data storage.
 * @return
 */
void filter_context(rd_t* rd);

/**
 * Filters allocations with resource types matching the specified resource filter.
 *
 * @param[in] rd   the resource trace data storage.
 * @return
 */
void filter_resource(rd_t* rd);


/**
 * Find lowest and highest allocation blocks.
 *
 * @param[in] rd  the resource trace data storage.
 * @return
 */
void filter_find_lowhigh_blocks(rd_t* rd);


/**
 * Sums memory leaks (allocated but not freed memory).
 *
 * The leaks are summed separately for every resource type.
 * And stored in the leaks[] array in the order the resources
 * are registered.
 * @param[in] call   the function call data.
 * @param[in] leaks  the leak summary data.
 * @return
 */
long filter_sum_leaks(rd_fcall_t* call, leak_data_t* leaks);


/**
 * Checks if only one resource type is present and sets
 * hide flag if necessary.
 *
 * @param[in] rd   the resource trace data storage.
 */
void filter_update_resource_visibility(rd_t* rd);


/**
 * Updates backtrace length to be less or equal to the specified size.
 * 
 * @param rd
 */
void filter_trim_backtraces(rd_t* rd);


/**
 * Performs include filter, removing call events not listed in the include file.
 *
 * @param[in] rd  the resource trace data storage.
 */
void filter_include(rd_t* rd);


/**
 * Performs exclude filter, removing call events listed in the exclude file.
 *
 * @param[in] rd  the resource trace data storage.
 */
void filter_exclude(rd_t* rd);


#endif /* FILTER_H*/
