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
#include "config.h"

#include "pages_report.h"
#include "options.h"

enum {
	PAGE_LEGEND_ZERO = 0,
	PAGE_LEGEND_NONDIRTY,
	PAGE_LEGEND_DIRTYZ,
	PAGE_LEGEND_DIRTY,
	PAGE_LEGEND_SWAPZ,
	PAGE_LEGEND_SWAP,

	PAGE_LEGEND_LAST,
};

typedef struct {
	unsigned char mark;
	const char* desc;
} report_legend_t;

static report_legend_t report_legend[] = {
	{' ', "non-dirty zero page"},
	{'.', "non-dirty page"},
	{'0', "zeroed dirty page in RAM"},
	{'#', "dirty page in RAM"},
	{'Z', "swapped zeroed page"},
	{'S', "swapped page"},
};

PagesReport::PagesReport(TraceData& data) :
		AddressSpaceReport(data)
{
}

void PagesReport::writeLegend(std::ostream& out)
{
	out << "\nLegend for the characters used in the memory mappings below:\n";
	for (int i = 0; i < PAGE_LEGEND_LAST; i++) {
		out << "  '" << report_legend[i].mark << "' : " << report_legend[i].desc << "\n";
	}
	out << "\n  " << PAGES_PER_LINE * Options::getInstance()->getPageSize() / 1024 << " KB per map line.\n";
	out << "\n\n";
}

void PagesReport::writeMemoryMap(std::ostream& out, MemoryArea* area)
{
	size_t pages_all = (area->to - area->from) / Options::getInstance()->getPageSize();
	size_t pages_dirty = 0;
	size_t pages_dirtyZ = 0;
	size_t pages_swap = 0;
	size_t pages_swapZ = 0;

	// write mapping graph
	out << std::hex << std::setfill('0');
	out << "          " << std::string( PAGES_PER_LINE, '-' ) << "\n";
	for (unsigned int page = 0; page < pages_all; page++) {
		if ( !(page % PAGES_PER_LINE) ) {
			if (page) out << "|\n";
			out << std::setw(8) << page * Options::getInstance()->getPageSize() + area->from << " |";
		}
		unsigned int page_mark = PAGE_LEGEND_NONDIRTY;
		pageflags_data_t* page_data = area->flags + page;
		if (page_data->info & PAGE_SWAP) {
			if (page_data->info & PAGE_ZERO) {
				page_mark = PAGE_LEGEND_SWAPZ;
				pages_swapZ++;
			}
			else {
				page_mark = PAGE_LEGEND_SWAP;
			}
			pages_swap++;
		}
		/* while swap backed pages are not technically dirty (from the kernel pov), they are written to
		 * and counted as dirty from application pov */
		else if (page_data->kflags & (BIT(DIRTY) | BIT(SWAPBACKED) | BIT(SWAPCACHE)) ) {
			if (page_data->info & PAGE_ZERO) {
				page_mark = PAGE_LEGEND_DIRTYZ;
				pages_dirtyZ++;
			}
			else {
				page_mark = PAGE_LEGEND_DIRTY;
			}
			pages_dirty++;
		}
		else if (page_data->info & PAGE_ZERO) {
			page_mark = PAGE_LEGEND_ZERO;
		}
		out << report_legend[page_mark].mark;
	}
	out << "|\n" << std::setfill(' ') << std::dec;
	out << "          " << std::string( PAGES_PER_LINE, '-' ) << "\n\n";

	// write statistics
	size_t page_sizeKB = Options::getInstance()->getPageSize() / 1024;
	out << "type:           pages:     kB:  of area:  of all writable:\n";

	out << "all          " << std::setw(8) << pages_all << std::setw(8) << pages_all * page_sizeKB << std::setw(8);
	out<< 100 << '%' << std::setw(8) << pages_all * 100 / total_pages << "%\n";

	if (pages_dirty) {
		out << "dirty RAM    " << std::setw(8) << pages_dirty << std::setw(8) << pages_dirty * page_sizeKB << std::setw(8);
		out << pages_dirty * 100 / pages_all << '%' << std::setw(8) << pages_dirty * 100 / total_pages << "%\n";
	}
	if (pages_dirtyZ) {
		out << "dirty RAM + Z" << std::setw(8) << pages_dirtyZ << std::setw(8) << pages_dirtyZ * page_sizeKB << std::setw(8);
		out << pages_dirtyZ * 100 / pages_all << '%' << std::setw(8) << pages_dirtyZ * 100 / total_pages << "%\n";
	}
	if (pages_swap) {
		out << "swapped      " << std::setw(8) << pages_swap << std::setw(8) << pages_swap * page_sizeKB << std::setw(8);
		out << pages_swap * 100 / pages_all << '%' << std::setw(8) << pages_swap * 100 / total_pages << "%\n";
	}
	if (pages_swapZ) {
		out << "swapped + Z  " << std::setw(8) << pages_swapZ << std::setw(8) << pages_swapZ * page_sizeKB << std::setw(8);
		out << pages_swapZ * 100 / pages_all << '%' << std::setw(8) << pages_swapZ * 100 / total_pages << "%\n";
	}
	int pages_ram = pages_all - pages_swap;
	out << "in RAM       "  << std::setw(8) << pages_ram << std::setw(8) << pages_ram * page_sizeKB << std::setw(8);
	out << pages_ram * 100 / pages_all << '%' << std::setw(8) << pages_ram * 100 / total_pages << "%\n";
	out << "\n";
}

