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

#ifndef SP_RTRACE_DEFS_H
#define SP_RTRACE_DEFS_H

/**
 * @file sp_rtrace_defs.h
 *
 * Common definitions used by sp-strace protocols.
 */

/**
* Resource behavior flags
*/
enum sp_rtrace_resource_flags_t {
	/* Resource alloc/free functions uses reference counting. This means that
	 * the subsequent allocation functions for the resource with the same resource 
	 * id increments the resource reference counter. Free functions decrements 
	 * the reference counter and the resource is assumed to be freed after the
	 * reference counter reaches zero. */
	SP_RTRACE_RESOURCE_REFCOUNT = 1 << 0,
	
	/* The last resource behavior flag */
	SP_RTRACE_RESOURCE_LAST_FLAG = SP_RTRACE_RESOURCE_REFCOUNT, 
			
	/* Default resource behavior flags */
	SP_RTRACE_RESOURCE_DEFAULT = 0 
 };



#endif
