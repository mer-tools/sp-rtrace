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

#ifdef __cplusplus
extern "C" {
#endif

/*
 * kernel page flags
 */

#define KPF_BYTES		8
#define PROC_KPAGEFLAGS		"/proc/kpageflags"

/* copied from kpageflags_read() */
#define KPF_LOCKED		0
#define KPF_ERROR		1
#define KPF_REFERENCED		2
#define KPF_UPTODATE		3
#define KPF_DIRTY		4
#define KPF_LRU			5
#define KPF_ACTIVE		6
#define KPF_SLAB		7
#define KPF_WRITEBACK		8
#define KPF_RECLAIM		9
#define KPF_BUDDY		10

/* [11-20] new additions in 2.6.31 */
#define KPF_MMAP		11
#define KPF_ANON		12
#define KPF_SWAPCACHE		13
#define KPF_SWAPBACKED		14
#define KPF_COMPOUND_HEAD	15
#define KPF_COMPOUND_TAIL	16
#define KPF_HUGE		17
#define KPF_UNEVICTABLE		18
#define KPF_HWPOISON		19
#define KPF_NOPAGE		20
#define KPF_KSM			21

/* [32-] kernel hacking assistances */
#define KPF_RESERVED		32
#define KPF_MLOCKED		33
#define KPF_MAPPEDTODISK	34
#define KPF_PRIVATE		35
#define KPF_PRIVATE_2		36
#define KPF_OWNER_PRIVATE	37
#define KPF_ARCH		38
#define KPF_UNCACHED		39

/* [48-] take some arbitrary free slots for expanding overloaded flags
 * not part of kernel API
 */
#define KPF_READAHEAD		48
#define KPF_SLOB_FREE		49
#define KPF_SLUB_FROZEN		50
#define KPF_SLUB_DEBUG		51

#define KPF_ALL_BITS		((uint64_t)~0ULL)
#define KPF_HACKERS_BITS	(0xffffULL << 32)
#define KPF_OVERLOADED_BITS	(0xffffULL << 48)
#define BIT(name)		(1ULL << KPF_##name)
#define BITS_COMPOUND		(BIT(COMPOUND_HEAD) | BIT(COMPOUND_TAIL))



typedef struct {
	const char mark;
	const char* desc;
	const char* info;
} page_flag_name_t;

extern page_flag_name_t page_flag_names[];

#ifdef __cplusplus
}
#endif
