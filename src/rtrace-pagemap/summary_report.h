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

#ifndef _SUMMARY_REPORT_H_
#define _SUMMARY_REPORT_H_


#include "trace_data.h"
#include "pagemap.h"

class SummaryReport {
private:
	typedef std::map<uint64_t, unsigned int> pagetype_map_t;

	TraceData& trace_data;

	/**
	 * Filters the required flags.
	 *
	 * @param[in] flags   the raw page flags.
	 * @return            the filtered page flags value.
	 */
	uint64_t filterFlags(uint64_t flags);

public:

	/**
	 * Creates a class instance.
	 */
	SummaryReport(TraceData& data);


	/**
	 * Appends page summary report to the existing pagemap report.
	 *
	 * @param[in] filename   the pagemap report file name.
	 */
	void append(const std::string& filename);



};


#endif

