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

#include "pagemap.h"
#include "common/formatter.h"

#include "options.h"

Options::Options() :
	report_legend(false),
	report_density(false),
	report_summary(false),
	page_size(getpagesize()),
	highest(0),
	lowest(0)
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
		"  -l           - memory area mapping legend. Displays information about memory\n"
	    "                 pages contained in the mapped areas.\n"
		"  -d           - allocation per page statistics. Displays percentage of\n"
		"                 active allocations for each page.\n"
		"  --highest NN - the number of highest allocations per area to print.\n"
		"  --lowest NN  - the number of lowest allocations per area to print.\n"
		"  -s           - summary about page types from all memory areas.\n"
		"  -h           - this help page.\n"
			;
}

void Options::parseCommandLine(int argc, char* const argv[])
{
	// command line options
	struct option long_options[] = {
			 {"in", 1, 0, 'i'},
			 {"out", 1, 0, 'o'},
			 {"legend", 0, 0, 'l'},
			 {"density", 0, 0, 'd'},
			 {"lowest", 1, 0, 'L'},
			 {"highest", 1, 0, 'H'},
			 {"summary", 0, 0, 's'},
			 {"help", 0, 0, 'h'},
			 {0, 0, 0, 0},
	};

	int opt;
	opterr = 0;
	while ( (opt = getopt_long(argc, argv, "i:o:hlds", long_options, NULL)) != -1) {
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

			case 'l': {
				report_legend = true;
				break;
			}

			case 'L': {
				lowest = atoi(optarg);
				break;
			}

			case 'H': {
				highest = atoi(optarg);
				break;
			}

			case 'd': {
				report_density = true;
				break;
			}

			case 's': {
				report_summary = true;
				break;
			}

			case '?': {
				throw std::runtime_error(Formatter() << "Unknown option: " << (char*)argv[optind - 1]);
			}
		}
	}
}

