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

#include "shared_pages_report.h"
#include "options.h"

SharedPagesReport::SharedPagesReport(TraceData& data) :
		AddressSpaceReport(data)
{
}

void SharedPagesReport::writeLegend(std::ostream& out)
{
	out << "\nLegend for the characters used in the memory mappings below:\n";
	out << "\t'0' - page not mapped\n";
	out << "\t'1' - page mapped once\n";
	out << "\t...\n";
	out << "\t'9' - page mapped 9 times\n";
	out << "\t'*' - page mapped 10 or more times\n";
	out << "\n  " << PAGES_PER_LINE * Options::getInstance()->getPageSize() / 1024 << " KB per map line.\n";
	out << "\n\n";
}

void SharedPagesReport::writeMemoryMap(std::ostream& out, MemoryArea* area)
{
	size_t pages_all = (area->to - area->from) / Options::getInstance()->getPageSize();

	// write mapping graph
	out << std::hex << std::setfill('0');
	out << "          " << std::string( PAGES_PER_LINE, '-' ) << "\n";
	for (unsigned int page = 0; page < pages_all; page++) {
		if ( !(page % PAGES_PER_LINE) ) {
			if (page) out << "|\n";
			out << std::setw(8) << page * Options::getInstance()->getPageSize() + area->from << " |";
		}
		pageflags_data_t* page_data = area->flags + page;

		out << (char)(page_data->kcount < 10 ? '0' + page_data->kcount : '*');
	}
	out << "|\n" << std::setfill(' ') << std::dec;
	out << "          " << std::string( PAGES_PER_LINE, '-' ) << "\n\n";
}


