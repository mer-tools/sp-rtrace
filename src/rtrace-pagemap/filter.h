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

#include "trace_data.h"

/**
 * Filters input report by leaving only allocations of the specified
 * memory area and done only in pages with the specified types.
 *
 * The filtering is done by preparing index file and passing this
 * file to sp-rtrace-postproc to generate the filtered report.
 */
class Filter {
private:

	// the input trace data
	TraceData& trace_data;

public:

	/**
	 * Creates a new class instance.
	 */
	Filter(TraceData& trace_data);

	/**
	 * Writes filtered input report into file.
	 *
	 * @param[in] filename   the report file name. The standard output
	 *                       is used if the filename is empty.
	 */
	void write(const std::string& filename);

};
