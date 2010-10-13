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

#include "common/utils.h"

/* the global call context mask */
static unsigned int context_mask = 0;

/*
 * The context_index and sp_context_registry aren't static to allow
 * tools to dump context registry contents into file.
 */

/* The next free call context_id. */
unsigned int context_index = 0;


/* context lock for thread synchronization */
static volatile int context_lock = 0;


/* The call context registry. */
char sp_context_registry[SP_CONTEXT_REGISTRY_SIZE][SP_CONTEXT_NAME_SIZE];

unsigned int sp_context_create(const char* name)
{
	unsigned int context_id = 0;
	
	while(!sync_bool_compare_and_swap(&context_lock, 0, 1));
	if (context_index < SP_CONTEXT_REGISTRY_SIZE - 1) {
		context_id = 1 << context_index;
		strncpy(sp_context_registry[context_index], name, SP_CONTEXT_NAME_SIZE);
		sp_context_registry[context_index][SP_CONTEXT_NAME_SIZE - 1] = '\0';
		context_index++;
	}
	context_lock = 0;
	return context_id;
}

void sp_context_enter(unsigned int context_id)
{
	if ( (1 << context_index) > context_id ) {
		context_mask |= context_id;
	}
}

void sp_context_exit(unsigned int context_id)
{
	if ( (1 << context_index) > context_id ) {
		context_mask &= (~context_id);
	}
}

unsigned int sp_context_get_mask()
{
	return context_mask;
}

unsigned int sp_context_get_count()
{
	return context_index;
}

