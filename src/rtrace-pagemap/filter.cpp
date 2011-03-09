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

#include "filter.h"

#include "options.h"
#include "memory_area.h"


class BaseFilter {
public:

	virtual bool validate(MemoryArea::ptr_t& area) {
		return true;
	}
};


class NameFilter : public BaseFilter {
private:
	std::string name;
public:
	NameFilter(const std::string& name) : name(name) {
	}

	bool validate(MemoryArea::ptr_t& area) {
		return area->path.find(name) != std::string::npos;
	}
};


class AddressFilter : public BaseFilter {
private:
	unsigned long address;
public:
	AddressFilter(unsigned long address) : address(address) {
	}

	bool validate(MemoryArea::ptr_t& area) {
		return area->from <= address && address < area->to;
	}
};

/*
 *
 */

Filter::Filter(TraceData& trace_data) :
	trace_data(trace_data)
{
}


void Filter::write(const std::string& filename)
{
	MemoryArea::vector_t areas;

	std::auto_ptr<BaseFilter> filter;
	if (!Options::getInstance()->getFilterName().empty()) {
		filter = std::auto_ptr<BaseFilter>(dynamic_cast<BaseFilter*>(new NameFilter(Options::getInstance()->getFilterName())));
	}
	else if (!Options::getInstance()->getFilterAddress() != 0) {
		filter = std::auto_ptr<BaseFilter>(dynamic_cast<BaseFilter*>(new AddressFilter(Options::getInstance()->getFilterAddress())));
	}
	else {
		filter = std::auto_ptr<BaseFilter>(new BaseFilter());
	}

	for(MemoryArea::vector_t::iterator iter = trace_data.memory_areas.begin(); iter != trace_data.memory_areas.end(); iter++) {
		if (filter.get()->validate(*iter)) {
			areas.push_back(*iter);
		}
	}

	for(MemoryArea::vector_t::iterator iter = areas.begin(); iter != areas.end(); iter++) {
		MemoryArea* area = iter->get();
		std::cout << ">> " << area->info << "\n";
	}

}
