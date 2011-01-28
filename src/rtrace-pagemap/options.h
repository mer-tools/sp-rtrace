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
	bool report_address_space;

	// memory page size
	int page_size;

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
	bool getReportAddressSpace() {
		return report_address_space;
	}

	/**
	 * Retrieves the memory page size.
	 *
	 * @return
	 */
	int getPageSize() {
		return page_size;
	}
};


#endif

