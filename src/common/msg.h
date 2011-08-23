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

#ifndef _MSG_H_
#define _MSG_H_

/**
 * @file io.h
 *
 * This file contains rtrace warning/error message API.
 */


enum msg_verbosity_t {
	MSG_NONE = 0,
	MSG_ERROR = 1,
	MSG_WARNING = 2,
	MSG_ALL = MSG_ERROR | MSG_WARNING,
};

/**
 * Sets the message verbosity level.
 *
 * @param level  the message verbosity level (see msg_verbosity_t enum)
 */
void msg_set_verbosity(unsigned int level);


/**
 * Prints error message.
 *
 * @param severity  the message severity (MSG_ERROR, MSG_WARNING).
 * @param format    the message format (see printf function specification)
 */
void msg_write(unsigned int severity, const char* format, ...);



#define msg_error(...)      msg_write(MSG_ERROR, __VA_ARGS__)

#define msg_warning(...)    msg_write(MSG_WARNING, __VA_ARGS__)


#endif

