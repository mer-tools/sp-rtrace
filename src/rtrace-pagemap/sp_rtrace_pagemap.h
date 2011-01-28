/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2011 by Nokia Corporation
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

#ifndef _SP_RTRACE_PAGEMAP_H_
#define _SP_RTRACE_PAGEMAP_H_

#include <stdint.h>

/**
 * Additional page information flags.
 */
enum pageinfo_t {
	PAGE_SWAP = 1 << 0,  //!< PAGE_SWAP
	PAGE_MEMORY = 1 << 1,//!< PAGE_MEMORY
	PAGE_ZERO = 1 << 2,  //!< PAGE_ZERO
};

/**
 * Memory page data.
 */
typedef struct {
	/* page information (pageinfo_t enum) */
	unsigned int info;
	/* page flags (from /proc/kpageflags */
	uint64_t kflags;
} pageflags_data_t;

/**
 * Memory page data header.
 *
 * This structure contains information about the memory area
 * described by the following page data.
 */
typedef struct {
	/* the memory are start address */
	unsigned long from;
	/* the memory area end address */
	unsigned long to;
	/* size of the following page data */
	unsigned int size;
} pageflags_header_t;


#endif


