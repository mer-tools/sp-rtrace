/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010,2011 by Nokia Corporation
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
#ifndef _PARSER_H_
#define _PARSER_H_

#include "timeline.h"

class Processor;

/**
 * Parses sp-rtrace text log.
 * 
 * This class parses sp-rtrace text log and reports
 * registered resources/contexts and resource allocations/frees.
 */
class Parser {
private:


	// the processor
	Processor* processor;
public:

	/**
	 * Parses the sp-rtrace log file.
	 * 
	 * @param[in] filename   the file to parse.
	 * @param[in] processor  the event processing processor.
	 */
	void parseFile(const std::string& filename, Processor* processor);

};


#endif
