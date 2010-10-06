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

#ifndef FORMATTER_H
#define FORMATTER_H

/**
 * @file formatter.h
 *
 * Contains text output formatting functions.
 * See PROTO_TEXT file for format specifications.
 *
 * Most of the function operates with resource data structures
 * rd_* defined in rtrace_data.h
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include "rtrace_data.h"
#include "header.h"

/**
 * Writes the log header consisting of key=value pairs.
 *
 * @param[in] header  the header key=value pair array.
 * @param[in] fp      the output stream.
 * @return            0 - success.
 */
int formatter_write_header(const header_t* header, FILE* fp);

/**
 * Writes memory map record.
 *
 * @param[in] mmap    the memory mapping data.
 * @param[in] fp      the output stream.
 * @return            0 - success.
 */
int formatter_write_mmap(const rd_mmap_t* mmap, FILE* fp);

/**
 * Writes function call record.
 *
 * @param[in] call   the function call data.
 * @param[in] fp     the output stream.
 * @return           0 - success.
 */
int formatter_write_fcall(const rd_fcall_t* call, FILE* fp);

/**
 * Writes backtrace record.
 *
 * @param[in] trace   the backtrace data.
 * @param[in] fp      the output stream.
 * @return            0 - success.
 */
int formatter_write_ftrace(const rd_ftrace_t* trace, FILE* fp);

/**
 * Writes context registry record.
 *
 * @param[in] context  the context data.
 * @param[in] fp       the output stream.
 * @return             0 - success.
 */
int formatter_write_context(const rd_context_t* context, FILE* fp);


/**
 * Writes resource registry record.
 *
 * @param[in] resource  the resource data.
 * @param[in] fp        the output stream.
 * @return              0 - success.
 */
int formatter_write_resource(const rd_resource_t* resource, FILE* fp);

/**
 * Writes comment record.
 *
 * @param[in] comment   the comment data.
 * @param[in] fp        the output stream.
 * @return              0 - success.
 */
int formatter_write_comment(const rd_comment_t* comment, FILE* fp);



/**
 * Writes function argument records.
 *
 * @param[in] args  a NULL terminated function argument textual interpretation array.
 * @param[in] fp    the output stream.
 * @return          0 - success.
 */
int formatter_write_fargs(const rd_fargs_t* args, FILE* fp);

/**
 * Writes formatted text.
 *
 * Note that the LF character is not appended automatically. So the comments must end
 * with '\n' when necessary.
 * @param[in] fp       the output stream.
 * @param[in] format   the text format.
 * @param[in] ...      the format data.
 * @return             0 - success.
 */
int formatter_printf(FILE* fp, const char* format, ...);


#endif

