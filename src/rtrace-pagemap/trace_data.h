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

#ifndef _TRACE_DATA_H_
#define _TRACE_DATA_H_

#include "pagemap.h"

#include "library/sp_rtrace_defs.h"

#include "memory_area.h"

class TraceData {
private:
	// the pageflags file descriptor
	int pageflags_fd;

	// the base address of pagefile mapping
	void* pageflags_addr;

	// size of the pageflags file
	size_t pageflags_size;

	/**
	 * Scans mapped memory area data from the maps and pageflags files.
	 */
	void scanMemoryAreas();

	/**
	 * Releases resources allocated during report parsing.
	 */
	void release();

	/**
	 * Scans the mapped pageflags file for the memory area and returns a pointer
	 * to the area page flags data.
	 *
	 * @param[in] from   the memory area start address.
	 * @param[in] to     the memory area end address.
	 * @return           a pointer to the area page flags data or NULL if the specified
	 *                   area was not found.
	 */
	void* getPageflagsData(unsigned long from, unsigned long to);

	/**
	 * Adds memory area.
	 *
	 * @param[in] from    the memory area start address.
	 * @param[in] to      the memory area end address.
	 * @param[in] data    the memory area data from maps file.
	 */
	void addMemoryArea(unsigned long from, unsigned long to, pageflags_data_t* page_data, const std::string& path, const std::string& data);


	/**
	 * Finds memory area covering the specified address.
	 *
	 * @param[in] address
	 * @return              the memory area or NULL if not found.
	 */
	MemoryArea* findMemoryArea(pointer_t address);


	void storeTrace(std::list<CallEvent*>& events, sp_rtrace_btframe_t* frames, int nframes);


public:

	// report header
	sp_rtrace_header_t header;

	// name of the maps file
	std::string filename_maps;

	// name of the page flags file
	std::string filename_pageflags;

	// array of the mapped rw memory areas
	MemoryArea::vector_t memory_areas;

	/**
	 * Creates a new class instance.
	 */
	TraceData();

	/**
	 * Destroys the class instance.
	 *
	 * @return
	 */
	~TraceData();

	/**
	 * Parses sp-rtrace report.
	 *
	 * @param[in] filename   the input report file name.
	 */
	void parseReport(const std::string& filename);

};


#endif

