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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "msg.h"

static unsigned int msg_verbosity = MSG_ALL;

void msg_write(unsigned int level, const char* format, ...)
{
	static char* prefixes[] = {
		"",
		"ERROR: ",
		"WARNING: ",
	};

	if (level & msg_verbosity) {
		va_list args;
		va_start(args, format);
		if (level < sizeof(prefixes) / sizeof(prefixes[0])) {
			fputs(prefixes[level], stderr);
		}
		vfprintf(stderr, format, args);
		va_end(args);
	}

}

void msg_set_verbosity(unsigned int level)
{
	msg_verbosity = level;
}

