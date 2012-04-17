/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010-2012 by Nokia Corporation
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
#include "config.h"

#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include "library/sp_rtrace_context.h"
#include "sp_context_impl.h"
#include "sp_rtrace_main.h"

#include "common/utils.h"


/**
 * Empty call context function, used when context library is not available.
 * @return
 */
static int empty_get_call_context(void)
{
	return 0;
}

int sp_rtrace_init_context(void)
{
	/* see: sp_rtrace_context.h */
	void* fn = dlsym(RTLD_DEFAULT, "sp_context_get_mask");
	if (fn) {
		sp_rtrace_get_call_context = fn;
	}
	return 0;
}

int (*sp_rtrace_get_call_context)(void) = empty_get_call_context;


/**
 * Override the default sp_context_create() implementation to report context
 * creation events.
 */
static unsigned int (*rt_context_create)(const char* name);

unsigned int sp_context_create(const char* name)
{
	sp_rtrace_context_t context = {.id = rt_context_create(name), .name = (char*)name};
	if (context.id) {
		sp_rtrace_write_context_registry(&context);
	}
	return context.id;
}

static unsigned int init_context_create(const char* name) 
{
	rt_context_create = dlsym(RTLD_NEXT, "sp_context_create");
	return rt_context_create(name);
}

static unsigned int (*rt_context_create)(const char* name) = init_context_create;
