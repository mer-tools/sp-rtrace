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
#include <config.h>
#include <dlfcn.h>
#include <stdio.h>

#include "libunwind_support.h"
#include "common/debug_log.h"

/* libunwind library handle */
static void* handle = NULL;

static const char* const libs[] = {
	"libunwind.so.7.0.0",
};

/*
 * Public API implementation
 */
fn_backtrace_t libunwind_initialize()
{
	unsigned i;
	fn_backtrace_t bt;

	for (i=0; i < sizeof(libs) / sizeof(libs[0]); ++i) {
		handle = dlopen(libs[i], RTLD_LAZY);
		if (handle)
			break;
	}

	if (!handle) {
		LOG("Failed to dlopen() libunwind");
		return NULL;
	}

	bt = dlsym(handle, "backtrace");
	if (!bt) {
		LOG("Failed to dlsym() 'backtrace' from libunwind");
		libunwind_release();
	}

	return bt;
}

int libunwind_release()
{
	if (handle) {
		dlclose(handle);
		handle = NULL;
	}
	return 0;
}
