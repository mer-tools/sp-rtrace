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

/**
 * @file sp_rtrace_filter.h
 *
 * Filter API provides functionality to filter the function call records
 * by their parameters.
 *
 */

#ifndef _SP_RTRACE_FILTER_H_
#define _SP_RTRACE_FILTER_H_

#include "sp_rtrace_defs.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum sp_rtrace_filter_type_t {
	SP_RTRACE_FILTER_TYPE_NONE = 0,
	SP_RTRACE_FILTER_TYPE_ALLOC = 1,
	SP_RTRACE_FILTER_TYPE_FREE = 2,
	SP_RTRACE_FILTER_TYPE_ALL = SP_RTRACE_FILTER_TYPE_ALLOC |
	                            SP_RTRACE_FILTER_TYPE_FREE,
};


typedef struct {
	/* the allocation type to match  */
	int type;
	/* the allocation size to match, -1 matches all sizes */
	int size;
} sp_rtrace_filter_t;

/**
 * Creates function call resource filter.
 *
 * @param[in] type  the function call type to match (see sp_rtrace_filter_type_t enum).
 * @return          the created filter.
 */
sp_rtrace_filter_t* sp_rtrace_filter_create(int type);

/**
 * Releases resources allocated by function call resource filter.
 * @param[in] filter    the filter to free.
 */
void sp_rtrace_filter_free(sp_rtrace_filter_t* filter);

/**
 * Parses size filter option.
 *
 * The size option can be given in one of following formats:
 *   <size> - where size is the matching resource size.
 * @param[in] filter  the filter.
 * @param[in] opt     string containing size filter option.
 */
void sp_rtrace_filter_parse_size_opt(sp_rtrace_filter_t* filter, const char* opt);

/**
 * Validates function call record.
 *
 * @param[in] filter  the filter.
 * @param[in] fcall   the record to validate.
 * @return            true if the record matches filter parameters.
 */
bool sp_rtrace_filter_validate(sp_rtrace_filter_t* filter, sp_rtrace_fcall_t* fcall);


#ifdef __cplusplus
}
#endif

#endif
