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

/**
 * @file header.h
 * 
 * This file provides text format header parsing/formatting functionality.
 */
#ifndef HEADER_H_
#define HEADER_H_

#include "sp_rtrace_defs.h"

/* header filter field tag index */
enum filter_index_t {
	FILTER_LEAKS,
	FILTER_COMPRESS,
	FILTER_RESOLVE,
	
	FILTER_MAX,
};

/* header filter field mask */
enum filter_mask_t {
	FILTER_MASK_LEAKS = 1 << FILTER_LEAKS,
	FILTER_MASK_COMPRESS = 1 << FILTER_COMPRESS,
	FILTER_MASK_RESOLVE = 1 << FILTER_RESOLVE,
	
	FILTER_MASK_RESET = ~(FILTER_MASK_COMPRESS),
};

extern const char* header_fields[SP_RTRACE_HEADER_MAX];


/**
 * Parses text into header fields.
 * 
 * @param[in] header   the header data.
 * @param[in] text     the text to parse
 * @return             0 - success.
 */
int header_read(sp_rtrace_header_t* header, const char* text);

/**
 * Frees the header resources.
 * 
 * @param[in] header   the header to free.
 */
void header_free(sp_rtrace_header_t* header);

/**
 * Retrieves filter mask from the header filter field.
 * @param[in] header  the header data.
 * @return            the filter mask (see filter_mask_t structure).
 */
unsigned int header_get_filter(sp_rtrace_header_t* header);

/**
 * Sets header filter field.
 * 
 * @param[in] header       the header data.
 * @param[in] filter_mask  the mask to set (see filter_mask_t structure).
 */
void header_set_filter(sp_rtrace_header_t* header, unsigned int filter_mask);



#endif /* HEADER_H_ */
