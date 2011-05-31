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

#include "sp_rtrace_filter.h"
#include "common/utils.h"

#define SIZE_UNDEFINED        (-1)

sp_rtrace_filter_t* sp_rtrace_filter_create(int type)
{
	sp_rtrace_filter_t* filter = malloc_a(sizeof(sp_rtrace_filter_t));
	filter->type = type;
	filter->size = SIZE_UNDEFINED;
	return filter;
}

void sp_rtrace_filter_free(sp_rtrace_filter_t* filter)
{
	free(filter);
}

void sp_rtrace_filter_parse_size_opt(sp_rtrace_filter_t* filter, const char* opt)
{
	if (opt) {
		filter->size = atoi(opt);
	}
}

#include <stdio.h>
bool sp_rtrace_filter_validate(sp_rtrace_filter_t* filter, sp_rtrace_fcall_t* fcall)
{
	/* function call type mapping to filter type */
	static int types[] = {SP_RTRACE_FILTER_TYPE_NONE,
			              SP_RTRACE_FILTER_TYPE_FREE,
			              SP_RTRACE_FILTER_TYPE_ALLOC,
	};

	/* check if record matches specified filter type */
	if (! (types[fcall->type] & filter->type) ) return false;

	/* check if record matches specified resource size */
	if (filter->size != SIZE_UNDEFINED && filter->size != fcall->res_size) return false;

	/* all rules passed, record matches */
	return true;
}

