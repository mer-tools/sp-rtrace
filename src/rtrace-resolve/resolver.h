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

#ifndef RESOLVER_H
#define RESOLVER_H

#include <bfd.h>
#include <stdbool.h>

#include "common/dlist.h"
#include "common/sp_rtrace_proto.h"
#include "sarray.h"

struct rs_mmap_t;

/**
 * The resolver cache record.
 */
typedef struct rs_cache_record_t {
	/* associated memory mapping record */
	struct rs_mmap_t* mmap;

	/* bfd resolving data */
	bfd* file;
	asymbol** symbols;
	long symcount;
	char* dbg_name;

	/* elf resolving data */
	int fd;
	void* image;
	size_t image_size;

} rs_cache_record_t;


/**
 * Memory mapping record for resolver cache.
 */
typedef struct rs_mmap_t {
	int id;

	char* module;
	pointer_t from;
	pointer_t to;

	bool is_absolute;

	rs_cache_record_t* cache;
	bool is_cache_owner;

	FILE* fin;
	FILE* fout;
} rs_mmap_t;


/**
 * The resolver cache data.
 */
typedef struct {
	/* memory mapping data */
	sarray_t mmaps;

	/* memory mapping record index providing
	 * id -> memmory mapping record association */
	struct rs_mmap_t** mmaps_index;
	int mmaps_limit;
	int mmaps_size;

	rs_cache_record_t cache;
} rs_cache_t;


/**
 * Resolves address.
 *
 * @param[in] rs        the resolver cache.
 * @param[in] address   the address to resolve.
 * @param[in] name      the old name. Empty string for previously unresolved addresses.
 * @return              the resolved address name.
 */
const char* rs_resolve_address(rs_cache_t* rs, pointer_t address, const char* name);

/**
 * Adds new memory mapping record to the resolver cache.
 *
 * @param[in] rs      the resolver cache.
 * @param[in] module  the module name.
 * @param[in] from    the start address.
 * @param[in] to      the end address.
 * @return            the added memory mapping record.
 */
rs_mmap_t* rs_mmap_add_module(rs_cache_t* rs, const char* module, pointer_t from, pointer_t to, bool single_cache);

/**
 * Retrieves memory mapping record covering the specified address.
 *
 * @param[in] mmaps  the memory mapping array.
 * @param[in] addr   the address.
 * @return           the memory mapping record.
 */
rs_mmap_t* rs_mmap_find_module(sarray_t* mmaps, pointer_t addr);


/**
 * Initializes the resolver cache.
 *
 * @param[in] rs    the resolver cache.
 * @param[in] size  the cache size.
 * @return
 */
void rs_cache_init(rs_cache_t* rs);

/**
 * Frees the resolver cache.
 *
 * @param[in] rs   the resolver cache.
 * @return
 */
void rs_cache_free(rs_cache_t* rs);


/**
 * Converts target path into its path in the host OS filesystem.
 *
 * Only full paths are converted. This is used for cross platform resolving,
 * when guest system root path is specified.
 * Note that the path is stored locally and next rs_host_path() call will
 * overwrite it. Copy the returned path if it needs to be stored for some
 * time.
 * @param path  the target path.
 * @param out   the host path.
 * @return
 */
const char* rs_host_path(const char* path);

#endif

