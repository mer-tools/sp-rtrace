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

#include "terminal.h"

class Processor;

class Options {
private:
	// the output file template
	std::string out_template;
	// the input file name
	std::string in_filename;
	// the output file extension
	std::string out_extension;
	// working directory
	std::string working_dir;

	// the output image scaling factor
	unsigned int scale_x;
	unsigned int scale_y;
	// the time slice for activity report (milliseconds)
	unsigned int slice;

	// the output terminal
	Terminal terminal;
	
	// the resource filter value
	std::string resource_filter;
	
	// description of the specified filters
	std::string filter_desc;

	// logarithmic scaling of y axis containing size values
	std::string logscale_size;

	/**
	 * Displays application usage instructions.
	 */
	void displayUsage();


	/**
	 * Splits filter option range value into min/max components.
	 *
	 * @param[in] range  the range value in format <min>-<max>.
	 * @param[out] min   the minimum range value.
	 * @param[out] max   the maximum range value.
	 */
	void splitRange(const char* range, std::string& min, std::string& max);

	/**
	 * Updates filter description.
	 *
	 * This method updates filter description string which is displayed
	 * in the second line of the report titles.
	 * @param[in] name   the filter name.
	 * @param[in] value  the filter value.
	 */
	void updateFilterDesc(const std::string& name, const std::string& value);

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
	void parseCommandLine(int argc, char* const argv[], Processor* processor);

	/**
	 * Retrieves output file name template.
	 *
	 * This template is used by generators to construct the
	 * output file name. Usually it has the following format:
	 * <output filename template>-<genrator id>.<file type extension>
	 * @return   the output file name template.
	 */
	const std::string& getOutTemplate() const {
		return out_template;
	}


	/**
	 * Retrieves input file name.
	 * @return  the input file name.
	 */
	const std::string& getInFilename() const {
		return in_filename;
	}

	/**
	 * Retrieves output filename extension.
	 *
	 * The output filename extension depends on the used terminal type.
	 * @return
	 */
	const std::string& getOutExtension() const {
		return out_extension;
	}

	/**
	 * Retrieves horizontal scaling factor.
	 *
	 * The horizontal scaling factor affects output report width.
	 * @return    the horizontal scaling factor in percents.
	 */
	unsigned int getScaleX() const {
		return scale_x;
	}

	/**
	 * Retrieves vertical scaling factor.
	 *
	 * The vertical scaling factor affects output report height.
	 * @return    the vertical scaling factor in percents.
	 */
	unsigned int getScaleY() const {
		return scale_y;
	}

	/**
	 * Retrieves the current terminal.
	 *
	 * @return
	 */
	const Terminal* getTerminal() const {
		return &terminal;
	}

	/**
	 * Retrieves short description of active filters.
	 *
	 * @return   the description of active filters.
	 */
	const std::string getFilterDescription() const {
		return filter_desc;
	}

	/**
	 * Retrieves the activity report time slice setting.
	 * 
	 * @return   the time slice for activity report.
	 */
	unsigned int getSlice() const {
		return slice;
	}

	/**
	 * Retrieves resource filter value.
	 * 
	 * @return   the resource filter.
	 */
	const std::string& getResourceFilter() const {
		return resource_filter;
	}


	/**
	 * Retrieves working directory.
	 *
	 * By default the output directory is used as working directory.
	 * @return   the working directory.
	 */
	const std::string& getWorkingDir() const {
		return working_dir;
	}


	/**
	 * Retrieves logscale value for size axis.
	 *
	 * @return
	 */
	const std::string& getLogscaleSize() const {
		return logscale_size;
	}
};


#endif
