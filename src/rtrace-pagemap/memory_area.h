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

#ifndef _MEMORY_AREA_H_
#define _MEMORY_AREA_H_

#include "pagemap.h"

#include "sp_rtrace_pagemap.h"

#include "library/sp_rtrace_defs.h"

#include "call_event.h"

class MemoryArea {
public:
	enum {
		READ = 1 << 0,
		WRITE = 1 << 1,
		EXECUTE = 1 << 2,
		SHARED = 1 << 3,
	};

	typedef std::tr1::shared_ptr<MemoryArea> ptr_t;

	typedef std::vector<ptr_t> vector_t;


	// the memory area start address
	unsigned long from;
	// the memory area end address
	unsigned long to;
	// address of the memory
	pageflags_data_t* flags;
	// the memory are information (taken from maps file)
	std::string info;
	// the access permissions, parsed from info strings
	unsigned int permissions;

	// list of allocation events inside area
	std::list<CallEvent::ptr_t> events;

	/**
	 * Creates a new class instance.
	 */
	MemoryArea(unsigned long from, unsigned long to, pageflags_data_t* flags, const std::string& info);

	/**
	 * Adds allocation event to area allocation list.
	 *
	 * @param[in] call  the allocation event to add.
	 * @return          reference to the added event.
	 */
	CallEvent* addEvent(sp_rtrace_fcall_t& call);


	/**
	 * Sorts the call events by they allocation addresses
	 */
	void sortEvents();

};

#endif
