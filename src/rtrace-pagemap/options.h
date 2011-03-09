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


#ifndef _OPTIONS_H_
#define _OPTIONS_H_

class Options {
private:
	// the output file name
	std::string out_filename;

	// the input file name
	std::string in_filename;

	// address space statistics report request
	bool report_pages;

	// density of allocations per page report
	bool report_density;

	// summary of page types from all areas
	bool report_summary;

	// memory page size
	size_t page_size;

	// number of highest allocations to print
	int top;

	// number of lowest allocations to print
	int bottom;

	// name of the area to filter
	std::string filter_name;

	// address of resource in the area to filter
	unsigned long filter_address;

	// page types to filter
	std::string filter_pagetype;

	// filter mode flag
	bool filter;


	/**
	 * Displays application usage instructions.
	 */
	void displayUsage();


	/**
	 * Displays filter usage instructions.
	 */
	void displayFilterUsage();

public:

	/**
	 * Creates a new class instance.
	 */
	Options();

	/**
	 * Returns the options object instance.
	 *
	 * @return   the Options instance.
	 */
	static Options* getInstance() {
		static Options instance;
		return &instance;
	}

	/**
	 * Parses command line arguments.
	 *
	 * @param argc
	 * @param argv
	 * @param[in] processor   the log processor.
	 */
	void parseCommandLine(int argc, char* const argv[]);

	/**
	 * Retrieves output file name.
	 *
	 * @return   the output file name.
	 */
	const std::string& getOutFilename() const {
		return out_filename;
	}


	/**
	 * Retrieves input file name.
	 * @return  the input file name.
	 */
	const std::string& getInFilename() const {
		return in_filename;
	}

	/**
	 * Checks address space report flag.
	 *
	 * @return   true if address space report is requested.
	 */
	bool getReportLegend() {
		return report_pages;
	}

	/**
	 * Retrieves the memory page size.
	 *
	 * @return
	 */
	size_t getPageSize() {
		return page_size;
	}

	/**
	 * Retrieves the number of lowest allocations per area to print.
	 */
	int getBottomAllocCount() {
		return bottom;
	}

	/**
	 * Retrieves the number of highest allocations per area to print.
	 */
	int getTopAllocCount() {
		return top;
	}

	/**
	 * Checks page allocation density report flag.
	 */
	bool getReportDensity() {
		return report_density;
	}

	/**
	 * Checks page summary report flag.
	 */
	bool getReportSummary() {
		return report_summary;
	}

	/**
	 * Retrieves the filtered area name
	 */
	const std::string getFilterName() {
		return filter_name;
	}

	/**
	 * Retrieves the filtered area ddress
	 */
	unsigned long getFilterAddress() {
		return filter_address;
	}

	/**
	 * Retrieves the filter page types.
	 */
	const std::string getFilterPagetype() {
		return filter_pagetype;
	}

	/**
	 * Retrieves filter mode flag.
	 */
	bool getFilter() {
		return filter;
	}
};


#endif

