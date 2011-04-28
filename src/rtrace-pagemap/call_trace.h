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


#ifndef _CALL_TRACE_T_
#define _CALL_TRACE_T_

#include "pagemap.h"

class CallTrace {
private:

	void release() {
		if (data.nframes) {
			if (data.frames) delete [] data.frames;
			if (data.resolved_names) {
				for (unsigned int i = 0; i < data.nframes; i++) {
					if (data.resolved_names[i]) free(data.resolved_names[i]);
				}
				delete [] data.resolved_names;
			}
		}
	}

public:

	typedef std::tr1::shared_ptr<CallTrace> ptr_t;

	sp_rtrace_ftrace_t data;

	CallTrace(sp_rtrace_ftrace_t& trace) :
		data(trace)
	{
	}

	~CallTrace()
	{
		release();
	}

	void write(std::ostream& out)
	{
		out << std::hex << std::setfill('0');
		for (unsigned int i = 0; i < data.nframes; i++) {
			out << "\t\t0x" << std::setw(sizeof(long)) << data.frames[i];
			if (data.resolved_names[i] && *data.resolved_names[i]) {
				out << " (" << data.resolved_names[i] << ")";
			}
			out << "\n";
		}
		out << "\n" << std::dec << std::setfill(' ');
	}

};

#endif
