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

page_flag_name_t page_flag_names[] = {
	[KPF_LOCKED]		= {'L', "locked"},
	[KPF_ERROR]		= {'E', "error"},
	[KPF_REFERENCED]	= {'R', "referenced"},
	[KPF_UPTODATE]		= {'U', "uptodate"},
	[KPF_DIRTY]		= {'D', "dirty"},
	[KPF_LRU]		= {'l', "lru"},
	[KPF_ACTIVE]		= {'A', "active"},
	[KPF_SLAB]		= {'S', "slab"},
	[KPF_WRITEBACK]		= {'W', "writeback"},
	[KPF_RECLAIM]		= {'I', "reclaim"},
	[KPF_BUDDY]		= {'B', "buddy"},

	[KPF_MMAP]		= {'M', "mmap"},
	[KPF_ANON]		= {'a', "anonymous"},
	[KPF_SWAPCACHE]		= {'s', "swapcache"},
	[KPF_SWAPBACKED]	= {'b', "swapbacked"},
	[KPF_COMPOUND_HEAD]	= {'H', "compound_head"},
	[KPF_COMPOUND_TAIL]	= {'T', "compound_tail"},
	[KPF_HUGE]		= {'G', "huge"},
	[KPF_UNEVICTABLE]	= {'u', "unevictable"},
	[KPF_HWPOISON]		= {'X', "hwpoison"},
	[KPF_NOPAGE]		= {'n', "nopage"},
	[KPF_KSM]		= {'x', "ksm"},

	[KPF_RESERVED]		= {'r', "reserved"},
	[KPF_MLOCKED]		= {'m', "mlocked"},
	[KPF_MAPPEDTODISK]	= {'d', "mappedtodisk"},
	[KPF_PRIVATE]		= {'P', "private"},
	[KPF_PRIVATE_2]		= {'p', "private_2"},
	[KPF_OWNER_PRIVATE]	= {'O', "owner_private"},
	[KPF_ARCH]		= {'h', "arch"},
	[KPF_UNCACHED]		= {'c', "uncached"},

	[KPF_READAHEAD]		= {'I', "readahead"},
	[KPF_SLOB_FREE]		= {'P', "slob_free"},
	[KPF_SLUB_FROZEN]	= {'A', "slub_frozen"},
	[KPF_SLUB_DEBUG]	= {'E', "slub_debug"},
};

