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
#include "config.h"

#include <memory.h>
#include <sys/mman.h>

#include "namecache.h"
#include "common/utils.h"

#define CACHE_BITS        16
#define CACHE_SIZE        (1 << CACHE_BITS)
#define CACHE_SLOT_COUNT  5
#define CACHE_TOTAL_SIZE	(CACHE_SIZE + CACHE_SLOT_COUNT)

typedef struct {
	namecache_t data;

	unsigned int hits;
	unsigned int stamp;

} namecache_rec_t;

static namecache_rec_t* cache = NULL;
static unsigned int access_counter = 1;

/**
 * Calculates address hash value
 *
 * @param addr[in]    the address.
 * @return            the calculated hash value.
 */
static int calc_hash(void* addr)
{
	unsigned long hash = 0;
	unsigned long value = (unsigned long)addr;
	while (value) {
		hash ^= value & (CACHE_SIZE - 1);
		value >>= 3;
	}
	return hash;
}

/*
 * Public API implementation
 */

namecache_t*
namecache_get_data(void* addr)
{
	if (cache) {
		int i;
		namecache_rec_t* rec = &cache[calc_hash(addr)];
		for (i = 0; i < CACHE_SLOT_COUNT; i++) {
			if (rec->data.addr == addr) {
				return &rec->data;
			}
			rec++;
		}
	}
	return NULL;
}

namecache_t*
namecache_add(void* address, const char* resolved_name)
{
	if (cache) {
		int i;
		namecache_rec_t* rec = &cache[calc_hash(address)];
		namecache_rec_t* freerec = rec;
		for (i = 0; i < CACHE_SLOT_COUNT && freerec->hits; i++) {
			if (rec->hits * (access_counter - freerec->stamp) < freerec->hits
					* (access_counter - rec->stamp)) {
				freerec = rec;
			}
			rec++;
		}
		if (freerec->data.name) free(freerec->data.name);
		freerec->data.name = strdup_a(resolved_name);
		freerec->data.addr = address;
		freerec->stamp = access_counter;
		freerec->hits = 0;
		return &freerec->data;
	}
	return NULL;
}

int namecache_remove_region( void* start, void* end )
{
	if (cache) {
		int i;
		for (i = 0; i < CACHE_TOTAL_SIZE; i++) {
			void* addr = cache[i].data.addr;
			if (addr >= start && addr <= end) {
				if (cache[i].data.name) free(cache[i].data.name);
				cache[i].data.addr = 0;
				cache[i].stamp = 0;
			}
		}
	}
	return 0;
}

int namecache_alloc(void)
{
	cache = calloc_a(CACHE_TOTAL_SIZE, sizeof(namecache_rec_t));
	return 0;
}

int namecache_release(void)
{
	if (cache) {
		int i;
		for (i = 0; i < CACHE_TOTAL_SIZE; i++) {
			if (cache[i].data.name) free(cache[i].data.name);
		}
		free(cache);
	}
	return 0;
}
