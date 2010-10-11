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
#include <config.h>
#include <dlfcn.h>
#include <stdio.h>

#include "libunwind_support.h"
#include "common/debug_log.h"


#ifdef USE_LUNWIND

#define UNW_LOCAL_ONLY
#include <libunwind.h>

typedef int (*unw_init_local_t)(unw_cursor_t*, unw_context_t*);
typedef int (*unw_step_t)(unw_cursor_t*);
typedef int (*unw_get_reg_t)(unw_cursor_t*, unw_regnum_t, unw_word_t *);


/* libunwind library handle */
static void* handle = NULL;

/* used libunwind functions */
static unw_init_local_t punw_init_local = NULL;
static unw_step_t punw_step = NULL;
static unw_get_reg_t punw_get_reg = NULL;


#ifdef __arm__
 #define ARCH arm
#else
 #define ARCH x86
#endif

#define STR2(x)			#x
#define STR(x)			STR2(x)

#define LUNW_SYM(x) 	"_U" STR(ARCH) "_" #x
#define LUNW_LIB()		"libunwind-" STR(ARCH) ".so.7.0.0"


/**
 * Retrieves function call stack trace.
 * 
 * This function is meant as direct replacement for libc6 backtrace() 
 * function - so it has the same signature.
 * @param[out] frames  the output buffer for frame addresses.
 * @param[in] size     the maximum number of frame addresses to retrieve.
 * @return             the number of frame addresses retrieved.
 */
static int libunwind_backtrace(void** frames, int size)
{
	void** ptr = frames;
	unw_cursor_t cursor; 
	unw_context_t uc;
	unw_word_t ip;

	if (unw_getcontext(&uc) != 0) {
	  return 0;
	}
	
	if (punw_init_local(&cursor, &uc) != 0) {
	  return 0;
	}

	while (size-- > 0 && punw_step(&cursor) > 0) {
	  punw_get_reg(&cursor, UNW_REG_IP, &ip);
	  *ptr++ = (void*)ip;
	}
	return ptr - frames;
}

/*
 * Public API implementation
 */
fn_backtrace_t libunwind_initialize()
{
	handle = dlopen(LUNW_LIB(), RTLD_LAZY);
	if (handle) {
		punw_init_local = (unw_init_local_t)dlsym(handle, LUNW_SYM(init_local));
		punw_step = (unw_step_t)dlsym(handle, LUNW_SYM(step));
		punw_get_reg = (unw_get_reg_t)dlsym(handle, LUNW_SYM(get_reg));
		if (!punw_init_local || !punw_step || !punw_get_reg) {
			LOG("Failed to load libunwind symbols");
			dlclose(handle);
			handle = NULL;
			return NULL;
		}
		return libunwind_backtrace;
	}
	LOG("Failed to open %s.so library", LUNW_LIB());
	return NULL;
}

int libunwind_release()
{
	if (handle) {
		dlclose(handle);
		handle = NULL;
	}
	return 0;
}

#else

fn_backtrace_t libunwind_initialize()
{
	LOG("Attempting to call libunwind_initialize() when libunwind was not available at compilation time");
	return NULL;
}

int libunwind_release()
{
	return 0;
}

#endif


