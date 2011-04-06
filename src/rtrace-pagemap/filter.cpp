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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "pagemap.h"

#include "filter.h"

#include "options.h"
#include "memory_area.h"
#include "common/header.h"
#include "common/formatter.h"
#include "rtrace_common.h"
#include "area_filter.h"

/**
 * The page type filter.
 *
 * This filter validates call events by page type of the involved
 * (allocated/freed) resource id.
 */
class PageFilter {
private:
	uint64_t mask;
public:

	/**
	 * Creates a new class instance.
	 *
	 * @param[in] page_types  the page types to filter. The page_types is a string
	 *                        value, consisting of the same marks as are used to
	 *                        display page type summary report (see page_flag_names
	 *                        array).
	 */
	PageFilter(const std::string& page_types) : mask (0) {
		for (unsigned int imark = 0; imark < page_types.length(); imark++) {
			char mark = page_types[imark];
			for (unsigned int iflag = 0; iflag < sizeof(mask) * 8; iflag++) {
				if (page_flag_names[iflag].mark == mark) {
					mask |= (1ull << iflag);
					break;
				}
			}
		}
	}


	/**
	 * Performs area/call event validation.
	 *
	 * This function validates call event by checking if the page type of resource id
	 * matches the page type mask specified in filter constructor,
	 */
	bool validateEvent(MemoryArea* area, CallEvent::ptr_t& event) {
		// no page mask was specified defaulting to all events validated
		if (!mask) return true;
		// event was not allocated in this area, fail validation.
		if (area->from < event->call.res_id || area->to >= event->call.res_id) return false;
		// acquire resource id allocation page flags and validate it.
		unsigned long page_index = (event->call.res_id - area->from) >> 12;
		pageflags_data_t* page_data = area->flags + page_index;
		return page_data->kflags & mask;
	}

};
/*
 *
 */

Filter::Filter(TraceData& trace_data) :
	trace_data(trace_data)
{
}


void Filter::write(const std::string& filename)
{
	// create area filter to make a list of target areas.
	std::auto_ptr<AreaFilter> filter;
	if (!Options::getInstance()->getFilterName().empty()) {
		filter = std::auto_ptr<AreaFilter>(dynamic_cast<AreaFilter*>(new PathFilter(Options::getInstance()->getFilterName())));
	}
	else if (Options::getInstance()->getFilterAddress() != 0) {
		filter = std::auto_ptr<AreaFilter>(dynamic_cast<AreaFilter*>(new AddressFilter(Options::getInstance()->getFilterAddress())));
	}
	else {
		filter = std::auto_ptr<AreaFilter>(new AreaFilter());
	}

	// construct a list of the target memory areas
	MemoryArea::vector_t areas;
	for(MemoryArea::vector_t::iterator iter = trace_data.memory_areas.begin(); iter != trace_data.memory_areas.end(); iter++) {
		if (filter.get()->validate(iter->get())) {
			areas.push_back(*iter);
		}
	}
	// The kernel page flag data is gathered after tracing has been done, therefore it's valid only for resources that
	// were still allocated. To get correct data first remove allocated and freed resources by applying --leaks post-processor
	// filter.
	if (!Options::getInstance()->getFilterPagetype().empty()) {
		if (! (header_get_filter(&trace_data.header) & FILTER_MASK_LEAKS) ) {
			std::cerr << "WARNING: applying page type filter should be done on reports processed with post-processor --leaks option\n";
		}
	}
	// Create temporary file for event index storage
	char tmp_name[PATH_MAX];
	sprintf(tmp_name, "%s/pagemap-index-XXXXXX", getenv("HOME"));
	int fd = mkstemp(tmp_name);
	if (fd == -1) {
		throw std::runtime_error(Formatter() << "Failed to create indexing file " << tmp_name << " (" << strerror(errno) << ")");
	}
	FILE* fp = fdopen(fd, "w");

	// go over interesting memory area list and prepare a list of allocations done in the pages of right type
	PageFilter page_filter(Options::getInstance()->getFilterPagetype());
	for(MemoryArea::vector_t::iterator iter = areas.begin(); iter != areas.end(); iter++) {
		MemoryArea* area = iter->get();
		for (std::list<CallEvent::ptr_t>::iterator iter = area->events.begin(); iter != area->events.end(); iter++) {
			sp_rtrace_fcall_t& call = iter->get()->call;
			if (page_filter.validateEvent(area, *iter)) {
				fprintf(fp, "%d\n", call.index);
			}
		}
	}
	fclose(fp);

	// call post-processor to apply the index filter
	int pid = fork();
	if (pid == 0) {
		const char* const args[] = {SP_RTRACE_POSTPROC, "-i", Options::getInstance()->getInFilename().c_str(), "--include", tmp_name,
				Options::getInstance()->getOutFilename().empty() ? NULL : "-o", Options::getInstance()->getOutFilename().c_str(), NULL};
		execvp(SP_RTRACE_POSTPROC, (char**)args);
		throw std::runtime_error(Formatter() << "ERROR: Failed to execute " << SP_RTRACE_POSTPROC << " (" << strerror(errno) << ")");
	}
	int status;
	waitpid(pid, &status, 0);

	// remove the index filter file name
	remove(tmp_name);

}
