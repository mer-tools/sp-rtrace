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
#include "common/rtrace_data.h"
#include "leaks_sort.h"

/**
 * Leaks sorting data.
 */
typedef struct {
	dlist_t* sorted;
	op_binary_t compare;
} leaks_sort_t;

/**
 * Counts number of leaks and the total size for all allocations with
 * the specified backtrace.
 *
 * @param[in] call_ref   the function call reference.
 * @param[in] trace_ref  the backtrace reference.
 * @return
 */
static long count_leaks(ref_node_t* call_ref, ftrace_ref_t* trace_ref)
{
	trace_ref->leak_count++;
	trace_ref->leak_size += ((rd_fcall_t*)call_ref->ref)->res_size;
	return 0;
}

/**
 * Inserts backtrace reference into output list in sorted order.
 *
 * @param[in] trace   the backtrace data.
 * @param[in] sdata   the sorting data.
 * @return
 */
static long sort_leak(rd_ftrace_t* trace, leaks_sort_t* sdata)
{
	ftrace_ref_t* ref = (ftrace_ref_t*)dlist_create_node(sizeof(ftrace_ref_t));
	ref->ref = trace;
	ref->leak_count = 0;
	ref->leak_size = 0;

	dlist_foreach2(&trace->calls, (op_binary_t)count_leaks, (void*)ref);
	dlist_add_sorted(sdata->sorted, ref, sdata->compare);
	return 0;
}

long leaks_compare_by_size_asc(ftrace_ref_t* tref1, ftrace_ref_t* tref2)
{
	return tref1->leak_size - tref2->leak_size;
}

long leaks_compare_by_size_desc(ftrace_ref_t* tref1, ftrace_ref_t* tref2)
{
	return tref2->leak_size - tref1->leak_size;
}

long leaks_compare_by_count_asc(ftrace_ref_t* tref1, ftrace_ref_t* tref2)
{
	return tref1->leak_count - tref2->leak_count;
}

long leaks_compare_by_count_desc(ftrace_ref_t* tref1, ftrace_ref_t* tref2)
{
	return tref2->leak_count - tref1->leak_count;
}

void leaks_sort(htable_t* htraces, dlist_t* sorted, op_binary_t compare)
{
	leaks_sort_t sort_data = {
			.sorted = sorted,
			.compare = compare,
	};
	htable_foreach2(htraces, (op_binary_t)sort_leak, (void*)&sort_data);
}
