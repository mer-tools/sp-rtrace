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

#ifndef _CALL_EVENT_H_
#define _CALL_EVENT_H_
#include "config.h"

#include "pagemap.h"

#include "library/sp_rtrace_defs.h"

#include "call_trace.h"

class CallEvent {
private:

	void release_args() {
		for (std::list<sp_rtrace_farg_t>::iterator iter = args.begin(); iter != args.end(); iter++) {
			if (iter->name) free(iter->name);
			if (iter->value) free(iter->value);
		}
		args.clear();
	}

	void release() {
		if (call.name) free(call.name);
		if (call.res_type_flag == SP_RTRACE_FCALL_RFIELD_NAME) free(call.res_type);

		release_args();
	}


public:
	typedef std::tr1::shared_ptr<CallEvent> ptr_t;

	sp_rtrace_fcall_t call;
	CallTrace::ptr_t trace;

	std::list<sp_rtrace_farg_t> args;

	CallEvent(sp_rtrace_fcall_t& call) :
		call(call)
	{
	}

	~CallEvent() {
		release();
	}

	void setTrace(CallTrace::ptr_t& trace) {
		this->trace = trace;
	}

	void addArg(sp_rtrace_farg_t& arg) {
		args.push_back(arg);
	}

	void write(std::ostream& out) {
		out << "\t" << call.index << ". ";
		if (call.context) out << '@' << std::hex << call.context << std::dec << " ";
		if (call.timestamp) {
			int hours = call.timestamp / (1000 * 60 * 60);
			int usecs = call.timestamp % (1000 * 60 * 60);
			int minutes = usecs / (1000 * 60);
			usecs %= 1000 * 60;
			int seconds = usecs / 1000;
			usecs %= 1000;
			out << std::setfill('0') << "[" << std::setw(2) << hours << ":" << std::setw(2) << minutes;
			out << ":" << std::setw(2) << seconds << "." << std::setw(3) << usecs << "] " << std::setfill(' ');
		}
		out << call.name << "(" << call.res_size << ") = 0x" << std::hex << call.res_id << std::dec << "\n";
		if (trace.get()) trace->write(out);
		else out << "\n";
	}
};

#endif
