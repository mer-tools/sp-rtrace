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

#include <sstream>
#include <stdlib.h>
#include <stdint.h>

#include "pagemap.h"
#include "common/formatter.h"

#include "options.h"

Options::Options() :
	report_pages(false),
	report_density(false),
	report_summary(false),
	page_size(getpagesize()),
	top(0),
	bottom(0),
	filter_address(0),
	filter(false)
{
}

void Options::displayUsage()
{
	std::cout <<
		"sp_rtrace_pagemap post-processor is used to display memory page statistics\n"
		"generated from sp-rtrace pagemap module reports.\n"
		"Usage: sp-rtrace-pagemap [<options>]\n"
		"Where <options> are:\n"
		"  -i <path>    - the input file path. Standard input used by default.\n"
		"  -o <path>    - the output directory. Standard output is used by default.\n"
		"  -p           - page type statistics. Displays information about memory\n"
	    "                 pages contained in the mapped areas.\n"
		"  -d           - allocation per page statistics. Displays percentage of\n"
		"                 active allocations for each page.\n"
		"  --highest NN - the number of highest allocations per area to print.\n"
		"  --lowest NN  - the number of lowest allocations per area to print.\n"
		"  -s           - summary about page types from all memory areas.\n"
		"  -N <name>    - filter report by area name.\n"
		"  -A <addr>    - filter report by an address inside memory area.\n"
	    "  -P <type>    - filter by the page type.\n"
		"  -h           - this help page. More information is available at\n"
		"                 --help-filter, --help-pages commands.\n"
			;
}

void Options::displayFilterUsage()
{
	std::cout <<
		"Report filters are used to strip events from sp-rtrace report based on\n"
		"allocated/freed resource page types/memory areas. The filters can be\n"
		"invoked only if no pagemap reports are requested. Otherwise filters are\n"
		"ignored. To generate filtered pagemap report first apply the necessary\n"
		"filter and then generate report from the filtered output.\n\n"
		"The filters leave only events with resource identifiers (addresses):\n"
		"-N <name> (--filter-name <name>) - belonging to the specified memory area.\n"
		"-A <addr> (--filter-address <addr>) - belonging to the same memory area\n"
		"   as the specified address.\n"
		"-P <type> (--filter-page <type>) - allocated on the memory pages matching\n"
		"   the requested page type. See --help-pages for description of memory\n"
		"   page types.\n"
		"The name filter overrides address filter while the page type filter can be\n"
		"used at the same time.\n"
		;
}


void Options::displayPageInfo()
{
	std::cout <<
		"Page type flags and description:\n\n";

	for (unsigned i = 0; i < sizeof(uint64_t) * 8; i++) {
		page_flag_name_t& page = page_flag_names[i];
		if (page.mark) {
			std::cout << (char)page.mark << ") " << page.desc << "\n";
			std::cout << page.info << "\n\n";
		}
	}


}

void Options::parseCommandLine(int argc, char* const argv[])
{
	// command line options
	struct option long_options[] = {
			 {"in", 1, 0, 'i'},
			 {"out", 1, 0, 'o'},
			 {"pages", 0, 0, 'p'},
			 {"density", 0, 0, 'd'},
			 {"bottom", 1, 0, 'B'},
			 {"top", 1, 0, 'T'},
			 {"summary", 0, 0, 's'},
			 {"filter-name", 1, 0, 'N'},
			 {"filter-address", 1, 0, 'A'},
			 {"filter-page", 1, 0, 'P'},
			 {"help", 0, 0, 'h'},
			 {"help-filter", 0, 0, 'H'},
			 {"help-pages", 0, 0, 'E'},
			 {0, 0, 0, 0},
	};

	int opt;
	opterr = 0;
	while ( (opt = getopt_long(argc, argv, "i:o:hpdsHA:P:N:T:B:", long_options, NULL)) != -1) {
		switch (opt) {
			case 'h': {
				displayUsage();
				exit(0);
			}

			case 'i': {
				in_filename = optarg;
				break;
			}

			case 'o': {
				out_filename = optarg;
				break;
			}

			case 'p': {
				if (filter) {
					std::cerr << "ERROR: filter option can't be used together with report options\n";
					exit (-1);
				}
				if (report_density) {
					std::cerr << "WARNING: density report option overrides pages option\n";
				}
				report_pages = true;
				break;
			}

			case 'B': {
				bottom = atoi(optarg);
				break;
			}

			case 'T': {
				top = atoi(optarg);
				break;
			}

			case 'd': {
				if (filter) {
					std::cerr << "ERROR: filter option can't be used together with report options\n";
					exit (-1);
				}
				if (report_density) {
					std::cerr << "WARNING: density report option overrides pages option\n";
				}
				report_density = true;
				break;
			}

			case 's': {
				report_summary = true;
				break;
			}

			case 'H': {
				displayFilterUsage();
				exit(0);
			}

			case 'E': {
				displayPageInfo();
				exit(0);
			}

			case 'A': {
				if (report_pages || report_density || report_summary) {
					std::cerr << "ERROR: filter option can't be used together with report options\n";
					exit (-1);
				}
				filter = true;
				std::stringstream(optarg) >> filter_address;
				break;
			}

			case 'P': {
				if (report_pages || report_density || report_summary) {
					std::cerr << "ERROR: filter option can't be used together with report options\n";
					exit (-1);
				}
				filter = true;
				filter_pagetype = optarg;
				break;
			}

			case 'N': {
				if (report_pages || report_density || report_summary) {
					std::cerr << "ERROR: filter option can't be used together with report options\n";
					exit (-1);
				}
				filter = true;
				filter_name = optarg;
				break;
			}

			case '?': {
				throw std::runtime_error(Formatter() << "Unknown option: " << (char*)argv[optind - 1]);
			}
		}
	}
}

