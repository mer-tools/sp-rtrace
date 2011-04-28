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
#include "timeline.h"

#include "options.h"

#include "report_data.h"
#include "processor.h"
#include "resource_registry.h"
#include "parser.h"
#include "timestamp.h"


int main(int argc, char* const argv[])
{
	try {
		Processor processor;
		Options::getInstance()->parseCommandLine(argc, argv, &processor);

		if (processor.getGeneratorCount() == 0) {
			throw std::runtime_error("No report type specified");
		}
		processor.initialize();

		Parser parser;
		parser.parseFile(Options::getInstance()->getInFilename(), &processor);
		// flush the unfreed allocation events stored in processor resource registry caches.
		processor.flushEventCache();
		// generate the reports
		processor.finalize();
	}
	catch (std::ifstream::failure e) {
		std::cerr << "File error: " << e.what() << "\n";
	}
	catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
	}
	return 0;
}
