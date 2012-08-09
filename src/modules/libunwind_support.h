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

/**
 * @file libunwind_support.h
 * 
 * Support for stack frame unwinding based on libunwind library.
 * The libunwind library is loaded dynamically to make it fully
 * optional. 
 */

#ifndef _LIBUNWIND_SUPPORT_H
#define _LIBUNWIND_SUPPORT_H

typedef int (*fn_backtrace_t)(void** frames, int size);

/**
 * Initializes libunwind support.
 * 
 * @return    the stack frame unwinding function based on libunwind.
 */
fn_backtrace_t libunwind_initialize(void);

/**
 * Releases resources allocated by libunwind_initialize().
 */
int libunwind_release(void);


#endif
