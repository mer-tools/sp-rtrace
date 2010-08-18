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
#include "config.h"

#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>

#include "sp_rtrace_context.h"

static void initialize();

/*
 * runtime context function references
 */
static int (*create_context)(const char* name);
static void (*enter_context)(int context_id);
static void (*exit_context)(int context_id);

/*
 * Empty context functions. Used when the original context implementation
 * functions are not available.
 */
static int empty_create_context(const char* name)
{
	return 0;
}

static void empty_enter_context(int context_id)
{
}

static void empty_exit_context(int context_id)
{
}


/*
 * Context function initialization routines.
 */
static int init_create_context(const char* name)
{
	initialize();
	return create_context(name);
}

static void init_enter_context(int context_id)
{
	initialize();
	enter_context(context_id);
}

static void init_exit_context(int context_id)
{
	initialize();
	exit_context(context_id);
}

/* Set the runtime context function references to initialization routines */
static int (*create_context)(const char* name) = init_create_context;
static void (*enter_context)(int context_id) = init_enter_context;
static void (*exit_context)(int context_id) = init_exit_context;

/**
 * Initializes the runtime context function references.
 *
 * This function attempts to set context function references to the original
 * context functions. If the original context implementation is not available
 * empty functions are used (in other words the context functionality is
 * disabled)
 * @return
 */
static void initialize()
{
	create_context = (int(*)(const char*))dlsym(RTLD_DEFAULT, "sp_context_create");
	enter_context = (void(*)(int))dlsym(RTLD_DEFAULT, "sp_context_enter");
	exit_context = (void(*)(int))dlsym(RTLD_DEFAULT, "sp_context_exit");

	if (create_context == NULL || enter_context == NULL ||
		exit_context == NULL) {
		create_context = empty_create_context;
		enter_context = empty_enter_context;
		exit_context = empty_exit_context;
	}
	else {
	}
}

/*
 * Public API implementation
 */
int sp_rtrace_context_try_create(const char* name)
{
	return create_context(name);
}


void sp_rtrace_context_try_enter(int context_id)
{
	enter_context(context_id);
}


void sp_rtrace_context_try_exit(int context_id)
{
	exit_context(context_id);
}


