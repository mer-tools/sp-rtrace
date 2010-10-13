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

#ifndef SP_RTRACE_CONTEXT_H
#define SP_RTRACE_CONTEXT_H

/**
 * @file sp_rtrace_context.h
 * Resource tracing client side API for call context implementation.
 */

/* the size of the call context name */
#define SP_CONTEXT_NAME_SIZE      32

/* The size of the call context registry. Currently the call context is
 * implemented as a unsigned int bitmask, the maximum size is sizeof(int) */
#define SP_CONTEXT_REGISTRY_SIZE  sizeof(int)

/* the context registry */
char sp_context_registry[SP_CONTEXT_REGISTRY_SIZE][SP_CONTEXT_NAME_SIZE];


/**
 * Creates call context mask.
 *
 * @param[in] name   the call context name.
 * @return           the call context id (the value of the bit in the context
 *                   mask - 1,2,4...). 0 is returned if the call context
 *                   creation failed.
 */
unsigned int sp_context_create(const char* name);

/**
 * Enters call context.
 *
 * @param[in] context_id   the call context id.
 * @return
 */
void sp_context_enter(unsigned int context_id);


/**
 * Exits call context.
 *
 * @param[in] context_id   the call context id.
 * @return
 */
void sp_context_exit(unsigned int context_id);


/**
 * Retrieves the current call context.

 * @return  the current call context.
 */
unsigned int sp_context_get_mask();

/**
 * Retrieves the number of registered contexts.

 * @return  the number of registered contexts.
 */
unsigned int sp_context_get_count();

#endif

