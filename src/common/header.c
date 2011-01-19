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

#include <stdio.h>
#include <string.h>

#include "library/sp_rtrace_defs.h"
#include "header.h"
#include "common/utils.h"

/* hader filter field tags */
const char* filter_tags[FILTER_MAX] = {
		"leaks",
		"compress",
		"resolve",
};


unsigned int header_get_filter(sp_rtrace_header_t* header)
{
	const char* filter = header->fields[SP_RTRACE_HEADER_FILTER];
	unsigned int filter_mask = 0;
	if (filter) {
		int i;
		for (i = 0; i < FILTER_MAX; i++) {
			if (strstr(filter, filter_tags[i])) {
				filter_mask |= (1 << i);
			}
		}
	}
	return filter_mask;
}

void header_set_filter(sp_rtrace_header_t* header, unsigned int filter_mask)
{
	if (header->fields[SP_RTRACE_HEADER_FILTER]) free(header->fields[SP_RTRACE_HEADER_FILTER]);
	if (filter_mask) {
		char filter_text[4096], *ptr = filter_text;
		int i;
		for (i = 0; i < FILTER_MAX; i++) {
			if (filter_mask & (1 << i)) {
				if (ptr != filter_text) *ptr++ = '|';
				ptr = stpcpy(ptr, filter_tags[i]);
			}
		}
		header->fields[SP_RTRACE_HEADER_FILTER] = strdup_a(filter_text);
	}
	else {
		header->fields[SP_RTRACE_HEADER_FILTER] = NULL;
	}
}
