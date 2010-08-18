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

/**
 * @file leaks_sort.h
 *
 * This file provides leaks sorting for --filter-leaks --compressed
 * options.
 */
#ifndef LEAKS_SORT_H
#define LEAKS_SORT_H

#include "common/dlist.h"
#include "common/htable.h"

/**
 * Backtrace reference data.
 */
typedef struct {
	dlist_node_t node;
	rd_ftrace_t* ref;
	int leak_count;
	int leak_size;
} ftrace_ref_t;

/**
 * Leak comparison function for sorting backtraces by total leak size
 * in ascending order.
 * @param[in] tref1   the first backtrace reference.
 * @param[in] tref2   the second backtrace reference.
 * @return            less than zero if the first leak is 'less' than
 *                    second.
 */
long leaks_compare_by_size_asc(ftrace_ref_t* tref1, ftrace_ref_t* tref2);

/**
 * Leak comparison function for sorting backtraces by total leak size
 * in descending order.
 * @param[in] tref1   the first backtrace reference.
 * @param[in] tref2   the second backtrace reference.
 * @return            less than zero if the first leak is 'less' than
 *                    second.
 */
long leaks_compare_by_size_desc(ftrace_ref_t* tref1, ftrace_ref_t* tref2);

/**
 * Leak comparison function for sorting backtraces by leak count
 * in ascending order.
 * @param[in] tref1   the first backtrace reference.
 * @param[in] tref2   the second backtrace reference.
 * @return            less than zero if the first leak is 'less' than
 *                    second.
 */
long leaks_compare_by_count_asc(ftrace_ref_t* tref1, ftrace_ref_t* tref2);

/**
 * Leak comparison function for sorting backtraces by leak count
 * in descending order.
 * @param[in] tref1   the first backtrace reference.
 * @param[in] tref2   the second backtrace reference.
 * @return            less than zero if the first leak is 'less' than
 *                    second.
 */
long leaks_compare_by_count_desc(ftrace_ref_t* tref1, ftrace_ref_t* tref2);

/**
 * Sorts backtraces of the leaked resources.
 *
 * This function creates backtrace reference list sorted by the specified
 * comparison function.
 * @param[in] htraces   the backtrace hash table.
 * @param[out] sorted   the output list.
 * @param[in] compare   the comparison function.
 */
void leaks_sort(htable_t* htraces, dlist_t* sorted, op_binary_t compare);


#endif

