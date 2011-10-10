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
#include "pages_report.h"
#include "density_report.h"
#include "shared_pages_report.h"
#include "summary_report.h"
#include "filter.h"

int main(int argc, char* const argv[])
{
	try {
		Options::getInstance()->parseCommandLine(argc, argv);

		TraceData trace_data;
		trace_data.parseReport(Options::getInstance()->getInFilename());

		std::auto_ptr<AddressSpaceReport> report;
		switch (Options::getInstance()->getReportType()) {
			case Options::REPORT_DENSITY:
				report = std::auto_ptr<AddressSpaceReport>(new DensityReport(trace_data));
				break;

			case Options::REPORT_PAGES:
				report = std::auto_ptr<AddressSpaceReport>(new PagesReport(trace_data));
				break;

			case Options::REPORT_SHARED_PAGES:
				report = std::auto_ptr<AddressSpaceReport>(new SharedPagesReport(trace_data));
				break;
		}
		if (report.get()) {
			report->write(Options::getInstance()->getOutFilename());

			if (Options::getInstance()->getReportSummary()) {
				SummaryReport summary(trace_data);
				summary.append(Options::getInstance()->getOutFilename());
			}
		}
		else if (Options::getInstance()->getFilter()) {
			if (Options::getInstance()->getBottomAllocCount() || Options::getInstance()->getTopAllocCount()) {
				std::cerr << "Warning: --top, --bottom options can be used only with sp-rtrace-pagemap report "
						"options (-c, -d, -p)!\n";

			}
			if (Options::getInstance()->getInFilename().empty()) {
				throw std::runtime_error("ERROR: filter can't be applied to reports processed from standard input\n");
			}
			Filter filter(trace_data);
			filter.write(Options::getInstance()->getOutFilename());
		}
		else {
			std::cerr << "No report type or filter option given\n";
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

