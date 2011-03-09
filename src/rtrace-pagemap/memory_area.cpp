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

#include "memory_area.h"

/**
 * Compares call events by the allocation address.
 *
 * @param[in] first   the first call event.
 * @param[in] second  the second call event.
 * @return            false, if the first event allocation address is less than second.
 */
static bool compare_res_id(CallEvent::ptr_t first, CallEvent::ptr_t second)
{
	if (first->call.res_id < second->call.res_id) return true;
	return false;
}

MemoryArea::MemoryArea(unsigned long from, unsigned long to, pageflags_data_t* flags, const std::string& path, const std::string& info) :
	from(from),
	to(to),
	flags(flags),
	info(info),
	permissions(0),
	path(path)
{
	int npos = info.find(' ');
	if (npos != -1) {
		if (info[npos + 1] == 'r') permissions |= READ;
		if (info[npos + 2] == 'w') permissions |= WRITE;
		if (info[npos + 3] == 'x') permissions |= EXECUTE;
		if (info[npos + 4] == 's') permissions |= SHARED;
	}
}


CallEvent* MemoryArea::addEvent(sp_rtrace_fcall_t& call)
{
	CallEvent::ptr_t event(new CallEvent(call));
	events.push_back(event);
	return event.get();
}


void MemoryArea::sortEvents()
{
	events.sort(compare_res_id);
}


