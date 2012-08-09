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

/**
 * Function call data structure.
 */
typedef struct module_fcall_t {
	/* the function call type (sp_rtrace_fcall_type_t structure) */
	unsigned int type;
	/* the function call timestamp */
	unsigned int timestamp;
	/* the function name */
	const char* name;
	/* the resource type identifier  */
	unsigned int res_type_id;
	/* the associated (allocated/freed) resource identifier */
	pointer_t res_id;
	/* the associated (allocated) resource size */
	int res_size;
} module_fcall_t;


/**
 * Function stack trace data
 */
typedef struct module_ftrace_t {
	/* number of frames in stack trace */
	unsigned long nframes;

	/* the frame return addresses */
	pointer_t* frames;

	/* The resolved address names. NULL when parsing binary data,
	 * but text files could contain already resolved names -
	 * in this case keep them intact. */
	char** resolved_names;
} module_ftrace_t;


/**
 * Call context data.
 */
typedef struct module_context_t {
	/* the context id */
	unsigned long id;
	/* the context name */
	const char* name;
} module_context_t;

/**
 * Resource type information.
 */
typedef struct module_resource_t {
	/* the resource type id */
	unsigned long id;
	/* the resource type */
	const char* type;
	/* the resource description */
	const char* desc;
	/* resource behaviour flags */
	unsigned int flags;
} module_resource_t;

/**
 * Function argument data
 */
typedef struct module_farg_t {
	/* the function argument name. Usually it matches the function argument prototype name */
	const char* name;
	/* the function argument value. */
	const char* value;
} module_farg_t;

/**
 * File attachment data
 */
typedef struct module_attachment_t {
	/* the attachment name */
	const char* name;
	/* name of the attached file */
	const char* path;
} module_attachment_t;


#endif /* SP_RTRACE_MODULE_H_ */
