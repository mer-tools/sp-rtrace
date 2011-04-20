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
#include <sys/mman.h>
#include <limits.h>

#include "pagemap.h"
#include "common/formatter.h"
#include "common/sp_rtrace_proto.h"
#include "library/sp_rtrace_parser.h"
#include "library/sp_rtrace_defs.h"

#include "sp_rtrace_pagemap.h"
#include "trace_data.h"

#define ATTACHMENT_MAPS	    "maps"
#define ATTACHMENT_PAGEFLAGS    "pageflags"

TraceData::TraceData() :
	pageflags_fd(0),
	pageflags_addr(NULL)
{
	memset(&header, 0, sizeof(sp_rtrace_header_t));
}

TraceData::~TraceData()
{
	release();
}

void TraceData::release()
{
	if (pageflags_addr && pageflags_addr != MAP_FAILED) munmap(pageflags_addr, pageflags_size);
	if (pageflags_fd > 0) close(pageflags_fd);
	pageflags_fd = 0;

	sp_rtrace_parser_free_header(&header);
}

void TraceData::addMemoryArea(unsigned long from, unsigned long to, pageflags_data_t* page_data, const std::string& path, const std::string& data)
{
	memory_areas.push_back(MemoryArea::ptr_t(new MemoryArea(from, to, page_data, path, data)));
}

void* TraceData::getPageflagsData(unsigned long from, unsigned long to)
{
	const pageflags_header_t* pheader = static_cast<pageflags_header_t*>(pageflags_addr);

	while (pheader->size + sizeof(pageflags_header_t) + ((char*)pheader - (char*)pageflags_addr) <= pageflags_size) {
		if (pheader->from == from && pheader->to == to) return (char*)pheader + sizeof(pageflags_header_t);
		pheader = (pageflags_header_t*)((char*)pheader + pheader->size + sizeof(pageflags_header_t));
	}
	return NULL;
}

void TraceData::scanMemoryAreas()
{
	std::ifstream file_maps(filename_maps.c_str());
	if (file_maps.fail()) {
		throw std::ios_base::failure(Formatter() << "Failed to open file: " << filename_maps);
	}
	char buffer[PATH_MAX];
	while (true) {
		file_maps.getline(buffer, sizeof(buffer));
		if (file_maps.eof()) break;
		unsigned long from, to;
		char rights[16];
		char path[PATH_MAX];
		if (sscanf(buffer, "%lx-%lx %[^ ] %[^ ] %[^ ] %[^ ] %[^ ]", &from, &to, rights, path, path, path, path) == 7) {
			void* ptr = getPageflagsData(from, to);
			addMemoryArea(from, to, static_cast<pageflags_data_t*>(ptr), path, buffer);
		}
	}
}

MemoryArea* TraceData::findMemoryArea(pointer_t address)
{
	for (MemoryArea::vector_t::iterator iter = memory_areas.begin(); iter != memory_areas.end(); iter++) {
		MemoryArea* area = iter->get();
		if (area->from <= address && area->to > address) return area;
	}
	return NULL;
}

/**
 * Stores trace data for the list of events.
 *
 * @param[in] events
 * @param[in] frames
 * @param[in] nframes
 */
void TraceData::storeTrace(std::list<CallEvent*>& events, sp_rtrace_btframe_t* frames, int nframes)
{
	if (events.empty()) return;

	sp_rtrace_ftrace_t trace = {nframes, NULL, NULL};
	if (nframes) {
		trace.frames = new pointer_t[nframes];
		trace.resolved_names = new char*[nframes];

		for (int i = 0; i < nframes; i++) {
			trace.frames[i] = frames[i].addr;
			trace.resolved_names[i] = frames[i].name;
		}
	}

	CallTrace::ptr_t call_trace(new CallTrace(trace));
	for (std::list<CallEvent*>::iterator iter = events.begin(); iter != events.end(); iter++) {
		(*iter)->setTrace(call_trace);
	}
}

void TraceData::parseReport(const std::string& filename)
{
	release();

	std::ifstream file;
	std::istream& in = filename.empty() ? std::cin : file;
	if (!filename.empty()) {
		file.open(filename.c_str(), std::ios_base::in);
		if (file.fail()) {
			throw std::ios_base::failure(Formatter() << "Failed to open file: " << filename);
		}
	}
	char buffer[4096];

	in.getline(buffer, sizeof(buffer));
	if (in.eof()) throw std::runtime_error(Formatter() << "Empty input file: " << filename);

	if (buffer[0] == (char)SP_RTRACE_PROTO_HS_ID) {
		throw std::runtime_error("Can't process sp-rtrace binary files. "
				                 "Convert to text format with sp-rtrace-postproc and try again.");
	}

	sp_rtrace_parser_parse_header(buffer, &header);

	while (true && (filename_maps.empty() || filename_pageflags.empty()) ) {
		in.getline(buffer, sizeof(buffer));
		if (in.eof()) break;

		sp_rtrace_record_t rec;
		int rec_type = sp_rtrace_parser_parse_record(buffer, &rec);
		switch (rec_type) {
			case SP_RTRACE_RECORD_ATTACHMENT:
				if (!strcmp(rec.attachment.name, ATTACHMENT_MAPS)) filename_maps = rec.attachment.path;
				else if (!strcmp(rec.attachment.name, ATTACHMENT_PAGEFLAGS)) filename_pageflags = rec.attachment.path;
				break;
		}
		sp_rtrace_parser_free_record(rec_type, &rec);
	}

	if (access(filename_maps.c_str(), F_OK) == -1) throw std::runtime_error(Formatter() << "Failed to access maps file '" << filename_maps << "': " << strerror(errno));
	if (access(filename_pageflags.c_str(), F_OK) == -1) throw std::runtime_error(Formatter() << "Failed to access pageflags file '" << filename_pageflags << "': " << strerror(errno));

	// map the pageflags file to memory
	pageflags_fd = open(filename_pageflags.c_str(), O_RDONLY);
	if (pageflags_fd == -1) throw std::runtime_error(Formatter() << "Failed to open pageflags file '" << filename_pageflags << "': " << strerror(errno));

	struct stat ps;
	if (fstat(pageflags_fd, &ps) ==  -1) throw std::runtime_error(Formatter() << "Failed to stat pageflags file '" << filename_pageflags << "': " << strerror(errno));
	pageflags_size = ps.st_size;

	pageflags_addr = mmap(NULL, pageflags_size, PROT_READ, MAP_SHARED, pageflags_fd, 0);
	if (pageflags_addr == MAP_FAILED) throw std::runtime_error(Formatter() << "Failed to mmap pageflags file '" << filename_pageflags << "': " << strerror(errno));

	// scan the mapped areas from maps/pageflagus files
	scanMemoryAreas();

	// scan the allocation events
	in.seekg(0);
	std::list<CallEvent*> last_events;

	sp_rtrace_btframe_t frames[512];
	sp_rtrace_btframe_t* pframe = frames;

	while (true) {
		in.getline(buffer, sizeof(buffer));
		if (in.eof()) break;

		sp_rtrace_record_t rec;
		int rec_type = sp_rtrace_parser_parse_record(buffer, &rec);
		if (rec_type == SP_RTRACE_RECORD_TRACE) {
			/* don't collect backtrace records if there is no preceeding allocation
			 * function event */
			if (last_events.empty()) {
				sp_rtrace_parser_free_record(rec_type, &rec);
			}
			else {
				*pframe++ = rec.frame;
			}
			continue;
		}
		// Store backtrace record for last cached events
		if ((pframe > frames || !*buffer) && !last_events.empty()) {
			storeTrace(last_events, frames, pframe - frames);
			last_events.clear();
		}

		if (rec_type == SP_RTRACE_RECORD_CALL) {
			if (rec.call.type == SP_RTRACE_FTYPE_ALLOC) {
				MemoryArea* area = findMemoryArea(rec.call.res_id);
				if (area) {
					last_events.push_back(area->addEvent(rec.call));
					continue;
				}
			}
		}
		pframe = frames;
		sp_rtrace_parser_free_record(rec_type, &rec);
	}
	if ((pframe > frames || !*buffer) && !last_events.empty()) {
		fprintf(stderr, "last parse\n");
		storeTrace(last_events, frames, pframe - frames);
		last_events.clear();
	}

	// sort the allocation events inside areas
	for (MemoryArea::vector_t::iterator iter = memory_areas.begin(); iter != memory_areas.end(); iter++) {
		iter->get()->sortEvents();
	}
}


