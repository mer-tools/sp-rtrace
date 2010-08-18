/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010 by Nokia Corporation
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
#ifndef WRITER_H
#define WRITER_H

/**
 * @file writer.h
 *
 * This file contains output generation functions.
 */

#include "common/rtrace_data.h"


/**
 * Wrapper for the data needed by formatting functions so
 * they can be passed as a single argument.
 */
typedef struct {
	FILE* fp;
	rd_t* rd;
	dlist_node_t* comment;
} fmt_data_t;


/**
 * Writes trace environment data into text log.
 *
 * The trace environment consists of log header, tracing module information,
 * context registry data, memory mapping data.
 * @param[in] rd    the resource trace data.
 * @param[in] fmt   the log file data.
 * @return
 */
void write_trace_environment(fmt_data_t* fmt);


/**
 * Writes memory leak summary.
 *
 * This function is called after the freed allocations has been
 * removed. Which means the rest of allocations is memory leaks
 * and should be reported as such.
 * @param[in] fmt  the formatter data.
 * @return
 */
void write_leak_summary(fmt_data_t* fmt);


/**
 * Writes function call trace.
 *
 * This function writes the filter function call trace data into output
 * stream.
 * @param[in] fmt  the formatter data.
 */
void write_trace_calls(fmt_data_t* fmt);




#endif /* WRITER_H */
