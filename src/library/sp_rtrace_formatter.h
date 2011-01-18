/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010,2011 by Nokia Corporation
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

struct sp_rtrace_fcall_t;
struct sp_rtrace_header_t;
struct sp_rtrace_mmap_t;
struct sp_rtrace_ftrace_t;
struct sp_rtrace_context_t;
struct sp_rtrace_resource_t;
struct sp_rtrace_farg_t;
struct sp_rtrace_attachment_t;

extern const char* sp_rtrace_resource_flags_text[SP_RTRACE_RESOURCE_FLAGS_MAX];

extern const char* header_fields[SP_RTRACE_HEADER_MAX];


/**
 * Prints report header.
 *
 * @param[in] fp        the output stream.
 * @param[in] header    in  - the report header.
 *                      out - timestamp (optional).
 * @return              0 - success, -errno - failure
 */
int sp_rtrace_print_header(FILE* fp, const struct sp_rtrace_header_t* header);

/**
 * Prints memory map information.
 *
 * @param[in] fp     the output stream.
 * @param[in] mmap   the memory mapping data.
 * @return           0 - success, -errno - failure
*/
int sp_rtrace_print_mmap(FILE* fp, const struct sp_rtrace_mmap_t* mmap);

/**
 * Prints function call information.
 *
 * @param[in] fp      the output stream.
 * @param[in] call    the function call data.
 * @return            0 - success, -errno - failure
 */
int sp_rtrace_print_call(FILE* fp, const struct sp_rtrace_fcall_t* call);


/**
 * Prints function stack trace.
 *
 * @param[in] fp      the output stream.
 * @param[in] trace   the stack trace data.
 * @return            0 - success, -errno - failure
 */
int sp_rtrace_print_trace(FILE* fp, const struct sp_rtrace_ftrace_t* trace);

/**
 * Prints single stack trace step.
 *
 * @param[in] fp        the output stream.
 * @param[in] addr      the frame return address.
 * @param[in] resolved  the resolved address name.
 * @return              0 - success, -errno - failure
 */
int sp_rtrace_print_trace_step(FILE* fp, void* addr, const const char* resolved);

/**
 * Prints context registry record.
 *
 * @param[in] fp        the output stream.
 * @param[in] context   the context data.
 * @return              0 - success, -errno - failure
 */
int sp_rtrace_print_context(FILE* fp, const struct sp_rtrace_context_t* context);


/**
 * Prints resource registry record.
 *
 * @param[in] fp        the output stream.
 * @param[in] resource  the resource data.
 * @return              0 - success, -errno - failure
 */
int sp_rtrace_print_resource(FILE* fp, const struct sp_rtrace_resource_t* resource);

/**
 * Prints comment.
 *
 * @param[in] fp      the output stream.
 * @param[in] format  the comment format template.
 * @param[in] ...     the template data.
 * @return            0 - success, -errno - failure
 */
int sp_rtrace_print_comment(FILE* fp, const char* format, ...);

/**
 * Prints function arguments.
 *
 * @param[in] fp     the output stream.
 * @param[in] args   the function argument array, ending with item containing NULLs.
 * @return           0 - success, -errno - failure
 */
int sp_rtrace_print_args(FILE* fp, const struct sp_rtrace_farg_t* args);

/**
 * Prints attachment information.
 *
 * @param[in] fp     the output stream.
 * @param[in] file   the attachment data.
 * @return           0 - success, -errno - failure
 */
int sp_rtrace_print_attachment(FILE* fp, const struct sp_rtrace_attachment_t* file);

#endif
