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

#include "address_space.h"

AddressSpace::AddressSpace()
{
}


void AddressSpace::addMemoryArea(unsigned long from, unsigned long to, pageflags_data_t* page_data, const std::string& data)
{
	memory_areas.push_back(MemoryArea::ptr_t(new MemoryArea(from, to, page_data, data)));
}


void AddressSpace::dump()
{
	for (MemoryArea::vector_t::iterator iter = memory_areas.begin(); iter != memory_areas.end(); iter++) {
		MemoryArea* area = iter->get();
		std::cout << std::hex << area->from << ", " << area->to << ", " << area->flags << std::dec << ", '" << area->info << "')\n";
	}
}


