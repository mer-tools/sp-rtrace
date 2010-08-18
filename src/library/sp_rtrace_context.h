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
 * Client side api for call context implementation.
 *
 * The sp_context_try_* functions tries to locate the context library
 * sp_contex_* functions and call them. If the original function
 * can't be located an empty (dummy) function is used instead.
 * This way the call context mechanism will be used when the
 * context library is available, but will not affect program
 * execution otherwise.
 */

/**
 * Tries to load and call sp_context_create function.
 *
 * This function creates a new call context with the specified
 * name and returns its identifier. The context API supports maximum of
 * 32 call contexts.
 * @param[in] name    the call context name.
 * @return            the call context id. The id is a bit value (1,2,4,8..).
 *                    0 means failure.
 */
int sp_rtrace_context_try_create(const char* name);

/**
 * Tries to load and call sp_context_enter function.
 *
 * This function activates the specified call context, which will be reported
 * in all consequent function call traces until deactivated.
 * @param[in] context_id   the call context id.
 * @return
 */
void sp_rtrace_context_try_enter(int context_id);

/**
 * Tries to load and call sp_context_exit function.
 *
 * This function deactivates the specified call context so it will not be reported
 * in function call traces anymore.
 * @param[in] context_id   the call context id.
 * @return
 */
void sp_rtrace_context_try_exit(int context_id);



#endif

