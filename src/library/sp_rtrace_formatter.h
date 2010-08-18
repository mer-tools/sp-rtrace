/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010 by Nokia Corporation
 *
 * Contact: Eero Tamminen <eero.tamminen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02r10-1301 USA
 */

/**
 * @file sp_rtrace_formatter.h
 *
 * API for printing rtrace text format compatible reports.
 */
#ifndef SP_RTRACE_FORMATTER_H
#define SP_RTRACE_FORMATTER_H

#include <sys/time.h>
#include <stdio.h>

/**
 * Prints report header.
 *
 * @param[in] fp            the output stream.
 * @param[in] version       the text format version.
 * @param[in] arch          the source system architecture.
 * @param[in] timestamp     the report creation time (if NULL the current time is used).
 * @param[in] pid           the traced process identifier.
 * @param[in] process_name  the traced process name.
 * @return
 */
void sp_rtrace_print_header(FILE* fp, const char* version, const char* arch,
		const struct timeval *timestamp, int pid, const char* process_name);

/**
 * Prints memory map information.
 *
 * @param[in] fp      the output stream.
 * @param[in] module  the module name.
 * @param[in] from    the starting memory address.
 * @param[in] to      the ending memory address.
 * @return
 */
void sp_rtrace_print_mmap(FILE* fp, const char* module, void* from, void* to);

/**
 * Prints function call information.
 *
 * @param[in] fp         the output stream.
 * @param[in] index      the function call index.
 * @param[in] context    the function call context.
 * @param[in] timestamp  the function call timestamp. Set 0 to omit timestamps
 *                       from function call records.
 * @param[in] name       the function name.
 * @param[in] res_size   the allocated resource size. Set 0 to specify deallocation call.
 * @param[in] res_id     the resource identifier.
 * @return
 */
void sp_rtrace_print_call(FILE* fp, int index, unsigned int context, unsigned int timestamp,
		const char* name, int res_size, void* res_id);


/**
 * Prints function backtrace.
 *
 * @param[in] fp        the output stream.
 * @param[in] frames    the frame addresses.
 * @param[in] resolved  the resolved address function names (can be NULL).
 * @param[in] nframes   the number of frame addresses.
 * @return
 */
void sp_rtrace_print_trace(FILE* fp, void** frames, char** resolved, int nframes);

/**
 * Prints single backtrace step.
 *
 * @param[in] fp        the output stream.
 * @param[in] addr      the frame return address.
 * @param[in] resolved  the resolved address name.
 * @return
 */
void sp_rtrace_print_trace_step(FILE* fp, void* addr, const char* resolved);

/**
 * Prints context registry record.
 *
 * @param[in] fp    the output stream.
 * @param[in] id    the context identifier.
 * @param[in] name  the context name.
 * @return
 */
void sp_rtrace_print_context(FILE* fp, unsigned int id, const char* name);

/**
 * Prints comment.
 *
 * @param[in] fp      the output stream.
 * @param[in] format  the comment format template.
 * @param[in] ...     the template data.
 * @return
 */
void sp_rtrace_print_comment(FILE* fp, const char* format, ...);

/**
 * Prints function arguments.
 *
 * @param[in] fp     the output stream.
 * @param[in] args   a NULL terminated function argument textual interpretation array.
 * @return
 */
void sp_rtrace_print_args(FILE* fp, char** args);


#endif