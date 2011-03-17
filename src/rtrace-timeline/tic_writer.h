/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010,2011 by Nokia Corporation
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
#ifndef _TIC_WRITER_H_
#define _TIC_WRITER_H_

#include "timeline.h"
#include "plotter.h"

class ITicWriter {
public:
	virtual void write(std::string& output, timestamp_t tic, Plotter::Tic& step) = 0;
};


class DefaultTicWriter : public ITicWriter {
private:
	timestamp_t start;

public:
	DefaultTicWriter() : start(0) {
	}

	void write(std::string& output, timestamp_t tic, Plotter::Tic& step) {
		if (!start) start = tic;
		output = Timestamp::toString(tic, step.decimal);
		output += "\\n+";
		output += Timestamp::offsetToString(tic - start);
	}
};


#endif
