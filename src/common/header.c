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

#include "header.h"
#include "common/utils.h"

/* header field names */
const char* header_fields[SP_RTRACE_HEADER_MAX] = {
		"version",
		"arch",
		"timestamp",
		"process",
		"pid",
		"filter",
		"backtrace depth",
};

/* hader filter field tags */
const char* filter_tags[FILTER_MAX] = {
		"leaks",
		"compress",
		"resolve",
};

int header_read(sp_rtrace_header_t* header, const char* text)
{
	char key[256], value[256];
	const char* ptr = text;
	int i;
	
	memset(header, 0, sizeof(sp_rtrace_header_t));
	
	while (sscanf(ptr, "%[^=]=%[^,]", key, value) == 2) {
		for (i = 0; i < SP_RTRACE_HEADER_MAX; i++) {
			if (!strcmp(key, header_fields[i])) { 
				if (header->fields[i]) free(header->fields[i]);
				header->fields[i] = strdup_a(value);
				break;
			}
		}
		/* advance pointer to the next field */
		ptr = strchr(ptr, ',');
		if (!ptr++) break;
		while (*ptr == ' ') ptr++;
	}
	return 0;
}

void header_free(sp_rtrace_header_t* header)
{
	int i;
	for (i = 0; i < SP_RTRACE_HEADER_MAX; i++) {
		if (header->fields[i]) free(header->fields[i]);
	}
}

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
