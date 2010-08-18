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

#ifndef SP_CONTEXT_IMPL_H
#define SP_CONTEXT_IMPL_H

/**
 * @file sp_context_impl.h
 *
 * Call context support.
 *
 * Call contexts are used to mark function contexts in tracing
 * tools from the target application.
 * This is done by:
 * 1) using use sp_context_try_* functions provided by libsp-context-dev.a
 *    library from the target application to create/enter/exit call
 *    contexts.
 * 2) linking libsp_context_impl.c file to the tracing tool.
 * 3) using sp_context_mask in tracing reports to identify call contexts.
 *
 * The sp_context_try_* functions attempts to load the sp_context_* functions
 * provided by libsp-context-impl-dev.a library. If the context implementation
 * library was not used, empty context manipulation functions are used.
 */

/* the size of the call context name */
#define SP_CONTEXT_NAME_SIZE      32

/* The size of the call context registry. Currently the call context is
 * implemented as a unsigned int bitmask, the maximum size is sizeof(int) */
#define SP_CONTEXT_REGISTRY_SIZE  sizeof(int)

extern unsigned int sp_context_mask;
/**
 * Creates call context flag.
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
void sp_contexta_enter(unsigned int context_id);


/**
 * Exits call context.
 *
 * @param[in] context_id   the call context id.
 * @return
 */
void sp_context_exit(unsigned int context_id);

#endif
