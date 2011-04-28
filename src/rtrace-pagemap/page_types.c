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

#include "page_types.h"

#define KPF_LOCKED_INFO         "Page is being locked for exclusive access, eg. by undergoing read/write IO."

#define KPF_ERROR_INFO          "IO error occurred."

#define KPF_REFERENCED_INFO     "Page has been referenced since last LRU list enqueue/requeue."

#define KPF_UPTODATE_INFO       "Page has up-to-date data (ie. for file backed page: (in-memory data revision >= on-disk one))."

#define KPF_DIRTY_INFO          "Page has been written to, hence contains new data (" \
                                "ie. for file backed page: (in-memory data revision >  on-disk one)_."

#define KPF_LRU_INFO            "Page is in one of the LRU lists."

#define KPF_ACTIVE_INFO         "Page is in the active LRU list."

#define KPF_SLAB_INFO           "Page is managed by the SLAB/SLOB/SLUB/SLQB kernel memory allocator " \
                                "When compound page is used, SLUB/SLQB will only set this flag on the head " \
                                "page; SLOB will not flag it at all."

#define KPF_WRITEBACK_INFO      "Page is being synced to disk."

#define KPF_RECLAIM_INFO        "Page will be reclaimed soon after its pageout IO completed."

#define KPF_BUDDY_INDO          "A free memory block managed by the buddy system allocator " \
                                "The buddy system organizes free memory in blocks of various orders. " \
                                "An order N block has 2^N physically contiguous pages, with the BUDDY flag " \
                                "set for and _only_ for the first page."

#define KPF_MMAP_INFO           "A memory mapped page."

#define KPF_ANON_INFO           "A memory mapped page that is not part of a file."

#define KPF_SWAPCACHE_INFO      "Page is mapped to swap space, ie. has an associated swap entry."

#define KPF_SWAPBACKED_INFO     "Page is backed by swap/RAM."

#define KPF_COMPOUND_INFO       "A compound page with order N consists of 2^N physically contiguous pages. " \
                                "A compound page with order 2 takes the form of \"HTTT\", where H donates its " \
                                "head page and T donates its tail page(s).  The major consumers of compound " \
                                "pages are hugeTLB pages (Documentation/vm/hugetlbpage.txt), the SLUB etc. " \
                                "memory allocators and various device drivers. However in this interface, " \
                                "only huge/giga pages are made visible to end users. "

#define KPF_HUGE_INFO           "This is an integral part of a HugeTLB page."

#define KPF_UNEVICTABLE_INFO    "Page is in the unevictable (non-)LRU list " \
                                "It is somehow pinned and not a candidate for LRU page reclaims, " \
                                "eg. ramfs pages, shmctl(SHM_LOCK) and mlock() memory segments."

#define KPF_HWPOISON_INFO       "Hardware detected memory corruption on this page: don't touch the data!"

#define KPF_NOPAGE_INFO         "No page frame exists at the requested address."

#define KPF_KSM_INFO            "Identical memory pages dynamically shared between one or more processes."

#define KPF_RESERVED_INFO       ""
#define KPF_MLOCKED_INFO        ""
#define KPF_MAPPEDTODISK_INFO   ""
#define KPF_PRIVATE_INFO        ""
#define KPF_PRIVATE_2_INFO      ""
#define KPF_PRIVATE_2_INFO      ""
#define KPF_OWNER_PRIVATE_INFO  ""
#define KPF_ARCH_INFO           ""
#define KPF_UNCACHED_INFO       ""
#define KPF_READAHEAD_INFO      ""
#define KPF_SLOB_FREE_INFO      ""
#define KPF_SLUB_FROZEN_INFO    ""
#define KPF_SLUB_DEBUG_INFO     ""

page_flag_name_t page_flag_names[] = {
	[KPF_LOCKED]		= {'L', "locked", KPF_LOCKED_INFO},

	[KPF_ERROR]		= {'E', "error", KPF_ERROR_INFO},
	[KPF_REFERENCED]	= {'R', "referenced", KPF_REFERENCED_INFO},
	[KPF_UPTODATE]		= {'U', "uptodate", KPF_UPTODATE_INFO},
	[KPF_DIRTY]		= {'D', "dirty", KPF_DIRTY_INFO},
	[KPF_LRU]		= {'l', "lru", KPF_LRU_INFO},
	[KPF_ACTIVE]		= {'A', "active", KPF_ACTIVE_INFO},
	[KPF_SLAB]		= {'S', "slab", KPF_SLAB_INFO},
	[KPF_WRITEBACK]		= {'W', "writeback", KPF_WRITEBACK_INFO},
	[KPF_RECLAIM]		= {'I', "reclaim", KPF_RECLAIM_INFO},
	[KPF_BUDDY]		= {'B', "buddy", KPF_BUDDY_INDO},
	[KPF_MMAP]		= {'M', "mmap", KPF_MMAP_INFO},
	[KPF_ANON]		= {'a', "anonymous", KPF_ANON_INFO},
	[KPF_SWAPCACHE]		= {'s', "swapcache", KPF_SWAPCACHE_INFO},
	[KPF_SWAPBACKED]	= {'b', "swapbacked", KPF_SWAPBACKED_INFO},
	[KPF_COMPOUND_HEAD]	= {'H', "compound_head", KPF_COMPOUND_INFO},
	[KPF_COMPOUND_TAIL]	= {'T', "compound_tail", KPF_COMPOUND_INFO},
	[KPF_HUGE]		= {'G', "huge", KPF_HUGE_INFO},
	[KPF_UNEVICTABLE]	= {'u', "unevictable", KPF_UNEVICTABLE_INFO},
	[KPF_HWPOISON]		= {'X', "hwpoison", KPF_HWPOISON_INFO},
	[KPF_NOPAGE]		= {'n', "nopage", KPF_NOPAGE_INFO},
	[KPF_KSM]		= {'x', "ksm", KPF_KSM_INFO},

	[KPF_RESERVED]		= {'r', "reserved", KPF_RESERVED_INFO},
	[KPF_MLOCKED]		= {'m', "mlocked", KPF_MLOCKED_INFO},
	[KPF_MAPPEDTODISK]	= {'d', "mappedtodisk", KPF_MAPPEDTODISK_INFO},
	[KPF_PRIVATE]		= {'P', "private", KPF_PRIVATE_INFO},
	[KPF_PRIVATE_2]		= {'p', "private_2", KPF_PRIVATE_2_INFO},
	[KPF_OWNER_PRIVATE]	= {'O', "owner_private", KPF_OWNER_PRIVATE_INFO},
	[KPF_ARCH]		= {'h', "arch", KPF_ARCH_INFO},
	[KPF_UNCACHED]		= {'c', "uncached", KPF_UNCACHED_INFO},

	[KPF_READAHEAD]		= {'I', "readahead", KPF_READAHEAD_INFO},
	[KPF_SLOB_FREE]		= {'P', "slob_free", KPF_SLOB_FREE_INFO},
	[KPF_SLUB_FROZEN]	= {'A', "slub_frozen", KPF_SLUB_FROZEN_INFO},
	[KPF_SLUB_DEBUG]	= {'E', "slub_debug", KPF_SLUB_DEBUG_INFO},
};

