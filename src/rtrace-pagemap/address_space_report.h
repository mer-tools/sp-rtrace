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
protected:

	enum {PAGES_PER_LINE = (16 * 3)};

	// the current memory area index
	int index;

	// the total number of pages mapped
	size_t total_pages;

	// the input trace data
	TraceData& trace_data;

	/**
	 * Writes memory area report.
	 *
	 * @param[in] area  the memory area.
	 */
	void writeMemoryArea(std::ostream& out, MemoryArea* area);


protected:


	/**
	 * Writes the memory map.
	 */
	virtual void writeMemoryMap(std::ostream& out, MemoryArea* area) = 0;

	/**
	 * Writes page markings legend.
	 *
	 * The page markings are single characters, used to display
	 * the page properties (dirty, swap etc) in address space
	 * statistics ascii representation.
	 */
	virtual void writeLegend(std::ostream& out) = 0;


	/**
	 * Validates the input data.
	 *
	 * This function is called before the report is generated to check
	 * if input file contains all necessary data and is in the right
	 * format.
	 * @return   true if the input data is complete.
	 */
	virtual bool validate();

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
