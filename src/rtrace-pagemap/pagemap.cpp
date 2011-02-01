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
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

#include "pagemap.h"
#include "options.h"
#include "trace_data.h"
#include "legend_report.h"
#include "density_report.h"

int main(int argc, char* const argv[])
{
	try {
		Options::getInstance()->parseCommandLine(argc, argv);

		TraceData trace_data;
		trace_data.parseReport(Options::getInstance()->getInFilename());

		AddressSpaceReport* report = NULL;
		if (Options::getInstance()->getReportDensity()) report = new DensityReport(trace_data);
		else if (Options::getInstance()->getReportLegend()) {
			report = new LegendReport(trace_data);
		}
		if (report) {
			report->write(Options::getInstance()->getOutFilename());
		}
		else {
			std::cerr << "No report type given\n";
		}
	}
	catch (std::ifstream::failure e) {
		std::cerr << "File error: " << e.what() << "\n";
	}
	catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
	}
	return 0;
}

