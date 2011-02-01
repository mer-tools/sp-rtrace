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

#include "density_report.h"
#include "options.h"

DensityReport::DensityReport(TraceData& data) :
		AddressSpaceReport(data)
{
}

void DensityReport::writeLegend(std::ostream& out)
{
	out << "\nLegend for the characters used in the memory mappings below:\n";
	out << "\t' ' - no page memory allocated\n";
	out << "\t'0' - 0% < allocation % of page <= 10%\n";
	out << "\t'1' - 10% < allocation % of page <= 20%\n";
	out << "\t...\n";
	out << "\t'9' - 90% < allocation % of page < 100%\n";
	out << "\t'#' - 100% of page allocated.\n";
	out << "\t'?' - more than 100% of page allocated - calculation error.\n";
	out << "\n  " << PAGES_PER_LINE * Options::getInstance()->getPageSize() / 1024 << " KB per map line.\n";
	out << "\n\n";
}

void DensityReport::writeMemoryMap(std::ostream& out, MemoryArea* area)
{
	size_t pages_all = (area->to - area->from) / Options::getInstance()->getPageSize();

	// write mapping graph
	out << std::hex << std::setfill('0');
	out << "          " << std::string( PAGES_PER_LINE, '-' ) << "\n";
	size_t overflow = 0;
	std::list<CallEvent::ptr_t>::iterator icall = area->events.begin();
	for (unsigned int page = 0; page < pages_all; page++) {
		unsigned long page_addr =  page * Options::getInstance()->getPageSize() + area->from;
		if ( !(page % PAGES_PER_LINE) ) {
			if (page) out << "|\n";
			out << std::setw(8) << page_addr << " |";
		}
		size_t size = 0;

		//std::cerr << "page_addr=" << std::hex << page_addr << std::dec << ", overflow=" << overflow << "\n";
		if (overflow < Options::getInstance()->getPageSize()) {
			size += overflow;
			overflow = 0;
			while (icall != area->events.end() && icall->get()->call.res_id < page_addr + Options::getInstance()->getPageSize()) {
				sp_rtrace_fcall_t& call = icall->get()->call;
				if (call.res_id + call.res_size > page_addr + Options::getInstance()->getPageSize()) {
					overflow = call.res_size - (page_addr + Options::getInstance()->getPageSize() - call.res_id);
					size += call.res_size - overflow;
					icall++;
					break;
				}
				else {
					size += call.res_size;
				}
				icall++;
			}
		}
		else {
			size = Options::getInstance()->getPageSize();
			overflow -= Options::getInstance()->getPageSize();
		}
		char mark = '?';
		if (size == 0) mark = ' ';
		else if (size == Options::getInstance()->getPageSize()) mark = '#';
		else if (size < Options::getInstance()->getPageSize()) mark = size * 10 / Options::getInstance()->getPageSize() + '0';
		else {
			std::cerr << "Warning, incorrect page allocation size calculated\n";
		}
		out << mark;
	}
	out << "|\n" << std::setfill(' ') << std::dec;
	out << "          " << std::string( PAGES_PER_LINE, '-' ) << "\n\n";
}

