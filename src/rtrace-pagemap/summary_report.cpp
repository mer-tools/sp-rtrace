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

#include "pagemap.h"

#include "common/formatter.h"
#include "library/sp_rtrace_defs.h"

#include "summary_report.h"
#include "options.h"

SummaryReport::SummaryReport(TraceData& data) :
	trace_data(data)
{
}


uint64_t SummaryReport::filterFlags(uint64_t flags)
{
	if (flags & BIT(SLAB)) {
		if (flags & BIT(PRIVATE)) flags ^= BIT(PRIVATE) | BIT(SLOB_FREE);
		if (flags & BIT(ACTIVE)) flags ^= BIT(ACTIVE) | BIT(SLUB_FROZEN);
		if (flags & BIT(ERROR)) flags ^= BIT(ERROR) | BIT(SLUB_DEBUG);
	}
	if ((flags & (BIT(RECLAIM) | BIT(WRITEBACK))) == BIT(RECLAIM)) flags ^= BIT(RECLAIM) | BIT(READAHEAD);

	/* hide flags intended only for kernel hacker */
	flags &= ~KPF_HACKERS_BITS;

	/* hide non-hugeTLB compound pages */
	if ((flags & BITS_COMPOUND) && !(flags & BIT(HUGE))) flags &= ~BITS_COMPOUND;

	return flags;
}


void SummaryReport::append(const std::string& filename)
{

	std::ofstream file;
	std::ostream& out = filename.empty() ? std::cout : file;
	if (!filename.empty()) {
		file.open(filename.c_str(), std::ios_base::out | std::ios_base::app);
		if (file.fail()) {
			throw std::ios_base::failure(Formatter() << "Failed to append to the file: " << filename);
		}
	}

	pagetype_map_t page_types;

	size_t total_pages = 0;
	for (MemoryArea::vector_t::iterator iter = trace_data.memory_areas.begin(); iter != trace_data.memory_areas.end(); iter++) {
		MemoryArea* area = iter->get();
		if (area->permissions & MemoryArea::WRITE) {
			total_pages += (area->to - area->from) / Options::getInstance()->getPageSize();
		}
	}

	for (MemoryArea::vector_t::iterator iter = trace_data.memory_areas.begin(); iter != trace_data.memory_areas.end(); iter++) {
		MemoryArea* area = iter->get();
		if (area->permissions & MemoryArea::WRITE) {
			size_t pages_all = (area->to - area->from) / Options::getInstance()->getPageSize();
			for (unsigned int page = 0; page < pages_all; page++) {
				pageflags_data_t* page_data = area->flags + page;
				uint64_t kflags = filterFlags(page_data->kflags);
				page_types[kflags]++;
			}
		}
	}

	uint64_t page_types_used = 0;
	out << "Memory page type summary\n"
		   "========================\n\n";
	out << std::setiosflags(std::ios_base::left) << std::setw(9) << "Count" << std::setw(65) << "Flags" << "Description\n";
	out << std::setiosflags(std::ios_base::right);
	for (pagetype_map_t::iterator iter = page_types.begin(); iter != page_types.end(); iter++) {
		out << std::setw(8) << iter->second << " ";
		std::string desc;
		for (unsigned int i = 0; i < 32; i++) {
			if (iter->first & (1ull << i)) {
				page_types_used |= (1ull << i);
				out << page_flag_names[i].mark;
				desc += ",";
				desc += page_flag_names[i].desc;
			}
			else out << '.';
		}
		out << ' ' << desc << "\n";
	}
	out << "\nLegend:\n";
	for (unsigned i = 0; i < sizeof(uint64_t) * 8; i++) {
		if (page_types_used & (1ull << i)) {
			out << "\t" << page_flag_names[i].mark << " : " << page_flag_names[i].desc << "\n";
		}
	}
}


