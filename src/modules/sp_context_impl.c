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
#include "config.h"

#include <stdlib.h>
#include <string.h>

#include "sp_context_impl.h"
#include "sp_rtrace_main.h"

#include "common/utils.h"

/* the global call context mask */
unsigned int sp_context_mask = 0;

/*
 * The sp_context_index and sp_context_registry aren't static to allow
 * tools to dump context registry contents into file.
 */

/* The next free call context_id. */
volatile size_t sp_context_index = 0;

/* The call context registry. */
char sp_context_registry[SP_CONTEXT_REGISTRY_SIZE][SP_CONTEXT_NAME_SIZE];

unsigned int sp_context_create(const char* name)
{
	int index = sync_fetch_and_add(&sp_context_index, 1);
	if (index >= SP_CONTEXT_REGISTRY_SIZE) {
		--sp_context_index;
		return 0;
	}
	strncpy(sp_context_registry[index], name, SP_CONTEXT_NAME_SIZE);
	sp_context_registry[index][SP_CONTEXT_NAME_SIZE - 1] = '\0';
	unsigned int context_id = 1 << index;
	sp_rtrace_write_context_registry(context_id, name);
	return context_id;
}

void sp_context_enter(unsigned int context_id)
{
	sp_context_mask |= context_id;
}

void sp_context_exit(unsigned int context_id)
{
	sp_context_mask &= (~context_id);
}

