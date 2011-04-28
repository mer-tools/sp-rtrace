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

/**
 * @file sp_rtrace_module.h
 *
 * This file contains common macros and constants used by tracing modules.
 */

#ifndef SP_RTRACE_MODULE_H_
#define SP_RTRACE_MODULE_H_

#include "common/utils.h"

/* backtrace synchronization variable, used in functions that are called by backtrace() */
extern __thread volatile sync_entity_t backtrace_lock;

/*
 * Synchronization macros to prevent recursive deadlocks if the tracked
 * function is used by libc backtrace() call.
 * These macros must be used for the functions, which are used by the
 * libc backtrace() function. Otherwise infinite recursion would happen.
 */

/**
 * Return the value of specified expression if the backtrace is locked
 * to current thread.
 *
 * Thread locks backtrace when it calls traced function. A traced function
 * call during backtrace lock means that the function is used by the
 * internal implementation and the original implementation must be used.
 *
 * This macro is used before calling the traced function to avoid recursive
 * deadlocks.
 */
#define BT_RETURN_IF_LOCKED(expression)   \
	if (backtrace_lock) {        \
		return expression;                \
	}

/**
 * Acquire backtrace lock, execute the specified expression and release
 * the acquired lock.
 *
 * This macro is used to execute the traced function.
 */
#define BT_LOCK_AND_EXECUTE(expression)   \
	expression;

/**
 * Attempt to acquire backtrace lock on current thread.
 * Return lock_fail_expression if failed, otherwise execute
 * lock_ok_expression and release backtrace lock.
 */
#define BT_EXECUTE_LOCKED(lock_ok_expression, lock_fail_expression) { \
		BT_RETURN_IF_LOCKED(lock_fail_expression);\
		BT_LOCK_AND_EXECUTE(lock_ok_expression);\
}

/**
 * Module initialization return codes
 */
enum {
	MODULE_UNINITIALIZED,//
	MODULE_LOADED,       // The standard functions were loaded, but sp_rtrace_initialize() failed
	MODULE_READY,        // The initialization was completed.
};

#endif /* SP_RTRACE_MODULE_H_ */
