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

#ifndef NAMECACHE_H
#define NAMECACHE_H

/**
 * @file namecache.h
 *
 * Resolved address names' cache.
 */

#define NAMECACHE_NAME_SIZE      512

/**
 * The name resolving data structure.
 */
typedef struct {
	void* addr;
	char* name;
} namecache_t;


/**
 * Retrieves cached record or NULL if not found.
 *
 * @param addr[in]    the address.
 * @return            the cached record or NULL if not found.
 */
namecache_t* namecache_get_data(void* addr );

/**
 * Caches the name data record.
 *
 * @param data[in]    the name data record;
 * @return
 */
namecache_t* namecache_add(void* address, const char* resolved_name);

/**
 * Removes addresses inside the specified region from the cache.
 *
 * This function is used when a new module is loaded, to ensure
 * that no old cache data is used.
 * @param start[in]  the start address.
 * @param end[in]    the end address.
 * @return           0 if successfull.
 */
int
namecache_remove_region(void* start, void* end );

/**
 * Allocates name cache.
 *
 * The name cache is allocated when logger is initialized with 'resolve' parameter set to true.
 * If the cache allocation failed, the resolve parameter is set to false.
 * @return    0 if successfull.
 */
int
namecache_alloc(void);

/**
 * Releases name cache.
 *
 * @return    0 if succssfull.
 */
int
namecache_release(void);

#endif /* NAMECACHE_H_ */
