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
 */

extern int (*sp_rtrace_get_call_context)();

/**
 * Initializes call context support.
 * 
 * This function tries to find already loaded sp_context_get_mask symbol of
 * libsp-rtrace1 library. If the symbol is not found - the empty_get_call_context()
 * function is used instead.
 * @return   the current call context.
 */
int sp_rtrace_init_context();


#endif
