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
 * Filters allocations with matching contexts.
 *
 * @param[in] rd   the resource trace data storage.
 * @return
 */
void filter_context(rd_t* rd);

/**
 * Filters allocations with matching module identifiers.
 *
 * @param[in] rd   the resource trace data storage.
 * @return
 */
void filter_module(rd_t* rd);


/**
 * Find lowest and highest allocation blocks.
 *
 * @param[in] rd  the resource trace data storage.
 * @return
 */
void find_lowhigh_blocks(rd_t* rd);


/**
 * Sums memory leaks (allocated but not freed memory).
 *
 * @param[in] call   the function call data.
 * @param[in] leaks  the leak summary data.
 * @return
 */
long sum_leaks(rd_fcall_t* call, leak_data_t* leaks);



#endif /* FILTER_H*/
