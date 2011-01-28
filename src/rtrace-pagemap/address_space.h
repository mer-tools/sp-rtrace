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

#ifndef _ADDRESS_SPACE_H_
#define _ADDRESS_SPACE_H_

#include "pagemap.h"
#include "sp_rtrace_pagemap.h"

#include "memory_area.h"

class AddressSpace {
public:
	// array of the mapped rw memory areas
	MemoryArea::vector_t memory_areas;

	/**
	 * Creates a new class instance.
	 */
	AddressSpace();

	/**
	 * Adds memory area.
	 *
	 * @param[in] from    the memory area start address.
	 * @param[in] to      the memory area end address.
	 * @param[in] data    the memory area data from maps file.
	 */
	void addMemoryArea(unsigned long from, unsigned long to, pageflags_data_t* page_data, const std::string& data);

	/**
	 * Dumps the address space contents.
	 *
	 * For debugging purpuses only.
	 */
	void dump();
};

#endif
