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

#include "address_space_report.h"
#include "options.h"
#include "area_filter.h"


AddressSpaceReport::AddressSpaceReport(TraceData& trace_data) :
	total_pages(0),
	trace_data(trace_data)
{
}

void AddressSpaceReport::writeMemoryArea(std::ostream& out, MemoryArea* area)
{
	out << index++ << ". " << area->info << "\n\n";

	writeMemoryMap(out, area);

	if (Options::getInstance()->getBottomAllocCount() && !area->events.empty()) {
		out << "Bottom " << Options::getInstance()->getBottomAllocCount() << " allocations:\n";
		std::list<CallEvent::ptr_t>::iterator iter = area->events.begin();
		for (int i = 0; i < Options::getInstance()->getBottomAllocCount() && iter != area->events.end(); i++) {
			iter->get()->write(out);
			iter++;
		}
		out << "\n";
	}
	if (Options::getInstance()->getTopAllocCount() && !area->events.empty()) {
		out << "Top " << Options::getInstance()->getTopAllocCount() << " allocations:\n";
		std::list<CallEvent::ptr_t>::iterator iter = area->events.end();
		for (int i = 0; i < Options::getInstance()->getTopAllocCount() && iter != area->events.begin(); i++) {
			iter--;
			iter->get()->write(out);
		}
		out << "\n";
	}

	out << "\n";
}


void AddressSpaceReport::write(const std::string& filename)
{
	if (!validate()) {
		throw std::runtime_error("Input data validation failed");
	}

	std::ofstream file;
	std::ostream& out = filename.empty() ? std::cout : file;
	if (!filename.empty()) {
		file.open(filename.c_str(), std::ios_base::out | std::ios_base::trunc);
		if (file.fail()) {
			throw std::ios_base::failure(Formatter() << "Failed to create file: " << filename);
		}
	}
	/* prepare area filter */
	std::auto_ptr<AreaFilter> filter;
	if (!Options::getInstance()->getFilterName().empty()) {
		filter = std::auto_ptr<AreaFilter>(dynamic_cast<AreaFilter*>(new PathFilter(Options::getInstance()->getFilterName())));
	}
	else if (Options::getInstance()->getFilterAddress() != 0) {
		filter = std::auto_ptr<AreaFilter>(dynamic_cast<AreaFilter*>(new AddressFilter(Options::getInstance()->getFilterAddress())));
	}
	else {
		filter = std::auto_ptr<AreaFilter>(new AreaFilter());
	}

	out << "SP-RTRACE PAGEMAP REPORT\n"
		   "========================\n\n"
			"Writable memory areas and their pages mapped to process:\n";
	out << "\t[" << trace_data.header.fields[SP_RTRACE_HEADER_PID] << "] " << trace_data.header.fields[SP_RTRACE_HEADER_PROCESS] << "\n\n";

	// write the memory area graph legend
	writeLegend(out);

	// count the total number of mapped pages
	total_pages = 0;
	for (MemoryArea::vector_t::iterator iter = trace_data.memory_areas.begin(); iter != trace_data.memory_areas.end();) {
		MemoryArea* area = iter->get();
		if (!filter->validate(area)) {
			iter = trace_data.memory_areas.erase(iter);
			continue;
		}
		if (area->permissions & MemoryArea::WRITE) {
			total_pages += (area->to - area->from) / Options::getInstance()->getPageSize();
		}
		iter++;
	}

	// write the memory area statistics
	index = 1;
	for (MemoryArea::vector_t::iterator iter = trace_data.memory_areas.begin(); iter != trace_data.memory_areas.end(); iter++) {
		MemoryArea* area = iter->get();
		if (area->permissions & MemoryArea::WRITE) {
			writeMemoryArea(out, area);
		}
	}

}


bool AddressSpaceReport::validate()
{
	return true;
}
