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
	report_address_space(false),
	page_size(getpagesize())

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
		"  -a           - address space statistics. Displays information about memory\n"
	    "                 pages contained in the mapped areas.\n"
		"  -h           - this help page.\n"
			;
}

void Options::parseCommandLine(int argc, char* const argv[])
{
	// command line options
	struct option long_options[] = {
			 {"in", 1, 0, 'i'},
			 {"out", 1, 0, 'o'},
			 {"address-space", 0, 0, 'a'},
			 {"help", 0, 0, 'h'},
			 {0, 0, 0, 0},
	};

	int opt;
	opterr = 0;
	while ( (opt = getopt_long(argc, argv, "i:o:ha", long_options, NULL)) != -1) {
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

			case 'a': {
				report_address_space = true;
				break;
			}

			case '?': {
				throw std::runtime_error(Formatter() << "Unknown option: " << (char*)argv[optind - 1]);
			}
		}
	}
}
