/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2011 by Nokia Corporation
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
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "sp_rtrace_filter.h"
#include "common/utils.h"

#define INITIAL_SIZESET_SIZE        32

#define SIZE_UNDEFINED        (-1)

sp_rtrace_filter_t* sp_rtrace_filter_create(int type)
{
	sp_rtrace_filter_t* filter = malloc_a(sizeof(sp_rtrace_filter_t));
	filter->type = type;
	filter->size_set = NULL;
	return filter;
}

void sp_rtrace_filter_free(sp_rtrace_filter_t* filter)
{
	if (filter->size_set) free(filter->size_set);
	free(filter);
}

void sp_rtrace_filter_parse_size_opt(sp_rtrace_filter_t* filter, const char* opt)
{
	if (opt) {
		filter->size_set = malloc_a(INITIAL_SIZESET_SIZE);
		int limit = INITIAL_SIZESET_SIZE;
		int size = 0;
		char* split;
		const char* delim = ",";
		char buffer[PATH_MAX];
		strcpy(buffer, opt);
		char* ptr = strtok_r(buffer, delim, &split);
		while (ptr) {
			if (size + 1 == limit) {
				limit <<= 1;
				filter->size_set = realloc_a(filter->size_set, limit);
			}
			filter->size_set[size++] = atoi(ptr);
			ptr = strtok_r(NULL, delim, &split);
		}
		filter->size_set[size++] = SIZE_UNDEFINED;
	}
}

bool sp_rtrace_filter_validate(const sp_rtrace_filter_t* filter, const sp_rtrace_fcall_t* fcall)
{
	/* function call type mapping to filter type */
	static int types[] = {SP_RTRACE_FILTER_TYPE_NONE,
			              SP_RTRACE_FILTER_TYPE_FREE,
			              SP_RTRACE_FILTER_TYPE_ALLOC,
	};

	/* check if record matches specified filter type */
	if (! (types[fcall->type] & filter->type) ) return false;

	/* check if record matches any of specified resource sizes */
	if (filter->size_set) {
		int* psize = filter->size_set;
		bool match = false;
		while (*psize != SIZE_UNDEFINED) {
			if (fcall->res_size == *psize++) {
				match = true;
				break;
			}
		}
		if (!match) return false;
	}

	/* all rules passed, record matches */
	return true;
}

