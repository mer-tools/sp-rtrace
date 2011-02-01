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
	bool report_legend;

	// density of allocations per page report
	bool report_density;

	// memory page size
	size_t page_size;

	// number of highest allocations to print
	int highest;

	// number of lowest allocations to print
	int lowest;

	/**
	 * Displays application usage instructions.
	 */
	void displayUsage();

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
	 * Retrieves address space report setting.
	 *
	 * @return   true if address space report is requested.
	 */
	bool getReportLegend() {
		return report_legend;
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
	int getLowestAllocCount() {
		return lowest;
	}

	/**
	 * Retrieves the number of highest allocations per area to print.
	 */
	int getHighestAllocCount() {
		return highest;
	}

	/**
	 * Retrieves page allocation density report setting
	 */
	int getReportDensity() {
		return report_density;
	}
};


#endif

