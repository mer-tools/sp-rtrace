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
#ifndef _ADDRESS_SPACE_REPORT_H_
#define _ADDRESS_SPACE_REPORT_H_

#include "trace_data.h"

class AddressSpaceReport
{
private:
	// the current memory area index
	int index;

	// the total number of pages mapped
	size_t total_pages;

	TraceData& trace_data;

	/**
	 * Writes memory area report.
	 *
	 * @param[in] area  the memory area.
	 */
	void writeMemoryArea(std::ostream& out, MemoryArea* area);

	/**
	 * Writes page markings legend.
	 *
	 * The page markings are single characters, used to display
	 * the page properties (dirty, swap etc) in address space
	 * statistics ascii representation.
	 */
	void writeLegend(std::ostream& out);

public:

	/**
	 * Creates a new class instance.
	 */
	AddressSpaceReport(TraceData& trace_data);

	/**
	 * Writes address space statistics report to the specified file.
	 *
	 * @param[in] filename   the report file name. The standard output
	 *                       is used if the filename is empty.
	 */
	void write(const std::string& filename);
};


#endif
