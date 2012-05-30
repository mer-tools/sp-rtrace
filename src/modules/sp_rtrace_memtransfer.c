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
#include "config.h"

/**
 * @file sp_rtrace_memtransfer.c
 *
 * memtransfer transfer tracing module (libsp-rtrace-memtransfer.so) implementation.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <wchar.h>
#include <unistd.h>
#include <execinfo.h>

#include "sp_rtrace_main.h"
#include "sp_rtrace_module.h"

#include "common/sp_rtrace_proto.h"
#include "library/sp_rtrace_defs.h"

/* glibc doesn't declare these for some reason even with _GNU_SOURCE,
 * declare them if they aren't macros.
 */
#ifndef strdupa
char *strdupa(const char *s);
#endif
#ifndef strndupa
char *strndupa(const char *s, size_t n);
#endif

/* make sure we don't catch glibc string.h wrapper macros */

#ifdef strdup
 #undef strdup
#endif

#ifdef strndup
 #undef strndup
#endif

#ifdef strncpy
 #undef strncpy
#endif

#ifdef strncat
 #undef strncat
#endif

#ifdef stpcpy
 #undef stpcpy
#endif

#ifdef mempcpy
 #undef mempcpy
#endif


/* Target function prototypes */
typedef char* (*strcpy_t)(char* dst, const char* src);
typedef void* (*mempcpy_t)(void *dest, const void *src, size_t n);
typedef void* (*memmove_t)(void *dest, const void *src, size_t n);
typedef void* (*memcpy_t)(void *dest, const void *src, size_t n);
typedef void* (*memset_t)(void *s, int c, size_t n);
typedef char* (*strncpy_t)(char *dest, const char *src, size_t n);
typedef char* (*stpcpy_t)(char *dest, const char *src);
typedef char* (*strcat_t)(char *dest, const char *src);
typedef char* (*strncat_t)(char *dest, const char *src, size_t n);
typedef void (*bcopy_t)(const void *src, void *dest, size_t n);
typedef void (*bzero_t)(void *s, size_t n);
typedef char* (*strdup_t)(const char *s);
typedef char* (*strndup_t)(const char *s, size_t n);
/* igore these if they're macros (like they're in glibc) */
#ifndef strdupa
typedef char* (*strdupa_t)(const char *s);
#endif
#ifndef strndupa
typedef char* (*strndupa_t)(const char *s, size_t n);
#endif
typedef wchar_t* (*wmemcpy_t)(wchar_t *dest, const wchar_t *src, size_t n);
typedef wchar_t* (*wmempcpy_t)(wchar_t *dest, const wchar_t *src, size_t n);
typedef wchar_t* (*wmemmove_t)(wchar_t* dest, const wchar_t* src, size_t b);
typedef wchar_t* (*wmemset_t)(wchar_t *s, wchar_t c, size_t n);
typedef wchar_t* (*wcscpy_t)(wchar_t *dest, const wchar_t *src);
typedef wchar_t* (*wcsncpy_t)(wchar_t *dest, const wchar_t *src, size_t n);
typedef wchar_t* (*wcpcpy_t)(wchar_t *dest, const wchar_t *src);
typedef wchar_t* (*wcpncpy_t)(wchar_t *dest, const wchar_t *src, size_t n);
typedef wchar_t* (*wcscat_t)(wchar_t *dest, const wchar_t *src);
typedef wchar_t* (*wcsncat_t)(wchar_t *dest, const wchar_t *src, size_t n);
typedef wchar_t* (*wcsdup_t)(const wchar_t *s);

/**
 * Target function references.
 */
typedef struct trace_t {
	strcpy_t strcpy;
	mempcpy_t mempcpy;
	memmove_t memmove;
	memcpy_t memcpy;
	memset_t memset;
	strncpy_t strncpy;
	stpcpy_t stpcpy;
	strcat_t strcat;
	strncat_t strncat;
	bcopy_t bcopy;
	bzero_t bzero;
	strdup_t strdup;
	strndup_t strndup;
#ifndef strdupa
	strdupa_t strdupa;
#endif
#ifndef strndupa
	strndupa_t strndupa;
#endif
	wmemcpy_t wmemcpy;
	wmempcpy_t wmempcpy;
	wmemmove_t wmemmove;
	wmemset_t wmemset;
	wcscpy_t wcscpy;
	wcsncpy_t wcsncpy;
	wcpcpy_t wcpcpy;
	wcpncpy_t wcpncpy;
	wcscat_t wcscat;
	wcsncat_t wcsncat;
	wcsdup_t wcsdup;
} trace_t;

/* original function references */
static trace_t trace_off;
/* tracing function references */
static trace_t trace_on;
/* tracing function initializers */
static trace_t trace_init;

/* Runtime function references */
static trace_t* trace_rt = &trace_init;

/* Initialization runtime function references */
static trace_t* trace_init_rt = &trace_off;

/* Module information */
static const sp_rtrace_module_info_t module_info = {
	.type = MODULE_TYPE_PRELOAD,
	.version_major = 1,
	.version_minor = 0,
	.symcount = sizeof(trace_t)/sizeof(pointer_t),
	.symtable = (const pointer_t*)&trace_off,
	.name = "memtransfer",
	.description = "Memory transfer operation tracking module. "
		       "Tracks functions that read/write memory blocks (strcpy, memset, wmemmove etc).",
};

static module_resource_t res_memtransfer = {
	.type = "memtransfer",
	.desc = "memory transfer operations in bytes",
	.flags = SP_RTRACE_RESOURCE_DEFAULT,
};


/**
 * Enables/disables tracing.
 *
 * @param[in] value   enable tracing if true, otherwise disable it.
 * @return
 */
static void enable_tracing(bool value)
{
	trace_rt = value ? &trace_on : &trace_off;
}


/**
 * Initializes original function references.
 *
 * @return
 */
static void trace_initialize(void)
{
	static int init_mode = MODULE_UNINITIALIZED;
	switch (init_mode) {
		case MODULE_UNINITIALIZED: {
			trace_off.strcpy = (strcpy_t)dlsym(RTLD_NEXT, "strcpy");
			trace_off.mempcpy = (mempcpy_t)dlsym(RTLD_NEXT, "mempcpy");
			trace_off.memmove = (memmove_t)dlsym(RTLD_NEXT, "memmove");
			trace_off.memcpy = (memcpy_t)dlsym(RTLD_NEXT, "memcpy");
			trace_off.memset = (memset_t)dlsym(RTLD_NEXT, "memset");
			trace_off.strncpy = (strncpy_t)dlsym(RTLD_NEXT, "strncpy");
			trace_off.stpcpy = (stpcpy_t)dlsym(RTLD_NEXT, "stpcpy");
			trace_off.strcat = (strcat_t)dlsym(RTLD_NEXT, "strcat");
			trace_off.strncat = (strncat_t)dlsym(RTLD_NEXT, "strncat");
			trace_off.bcopy = (bcopy_t)dlsym(RTLD_NEXT, "bcopy");
			trace_off.bzero = (bzero_t)dlsym(RTLD_NEXT, "bzero");
			trace_off.strdup = (strdup_t)dlsym(RTLD_NEXT, "strdup");
			trace_off.strndup = (strndup_t)dlsym(RTLD_NEXT, "strndup");
#ifndef strdupa
			trace_off.strdupa = (strdupa_t)dlsym(RTLD_NEXT, "strdupa");
#endif
#ifndef strndupa
			trace_off.strndupa = (strndupa_t)dlsym(RTLD_NEXT, "strndupa");
#endif
			trace_off.wmemcpy = (wmemcpy_t)dlsym(RTLD_NEXT, "wmemcpy");
			trace_off.wmempcpy = (wmempcpy_t)dlsym(RTLD_NEXT, "wmempcpy");
			trace_off.wmemmove = (wmemmove_t)dlsym(RTLD_NEXT, "wmemmove");
			trace_off.wmemset = (wmemset_t)dlsym(RTLD_NEXT, "wmemset");
			trace_off.wcscpy = (wcscpy_t)dlsym(RTLD_NEXT, "wcscpy");
			trace_off.wcsncpy = (wcsncpy_t)dlsym(RTLD_NEXT, "wcsncpy");
			trace_off.wcpcpy = (wcpcpy_t)dlsym(RTLD_NEXT, "wcpcpy");
			trace_off.wcpncpy = (wcpncpy_t)dlsym(RTLD_NEXT, "wcpncpy");
			trace_off.wcscat = (wcscat_t)dlsym(RTLD_NEXT, "wcscat");
			trace_off.wcsncat = (wcsncat_t)dlsym(RTLD_NEXT, "wcsncat");
			trace_off.wcsdup = (wcsdup_t)dlsym(RTLD_NEXT, "wcsdup");
			init_mode = MODULE_LOADED;

			LOG("module loaded: %s (%d.%d)", module_info.name, module_info.version_major, module_info.version_minor);
		}

		case MODULE_LOADED: {
			if (sp_rtrace_initialize()) {
				sp_rtrace_register_module(&module_info, enable_tracing);
				sp_rtrace_register_resource(&res_memtransfer);
				trace_init_rt = trace_rt;
				init_mode = MODULE_READY;

				LOG("module ready: %s (%d.%d)", module_info.name, module_info.version_major, module_info.version_minor);
			}
		}
	}
}

/*
 * Tracing functions
 *
 * NOTE: If strlen, wcslen functions are added to tracing function list, then
 * internal strlen, wcslen calls must be replaced with trace_off.strlen,
 * trace_off.wcslen calls.
 */
static char* trace_strcpy(char* dst, const char* src)
{
	char* rc = trace_off.strcpy(dst, src);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "strcpy",
			.res_size = strlen(src),
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static void* trace_mempcpy(void *dest, const void *src, size_t n)
{
	void* rc = trace_off.mempcpy(dest, src, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "mempcpy",
			.res_size = n,
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static void* trace_memmove(void *dest, const void *src, size_t n)
{
	void* rc = trace_off.memmove(dest, src, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "memmove",
			.res_size = n,
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static void* trace_memcpy(void *dest, const void *src, size_t n)
{
	void* rc = trace_off.memcpy(dest, src, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "memcpy",
			.res_size = n,
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static void* trace_memset(void *s, int c, size_t n)
{
	void* rc = trace_off.memset(s, c, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "memset",
			.res_size = n,
			.res_id = (pointer_t)s,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static char* trace_strncpy(char *dest, const char *src, size_t n)
{
	char* rc = trace_off.strncpy(dest, src, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "strncpy",
			.res_size = n,
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static char* trace_stpcpy(char *dest, const char *src)
{
	char* rc = trace_off.stpcpy(dest, src);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "stpcpy",
			.res_size = strlen(src),
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static char* trace_strcat(char *dest, const char *src)
{
	char* rc = trace_off.strcat(dest, src);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "strcat",
			.res_size = strlen(src),
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static char* trace_strncat(char *dest, const char *src, size_t n)
{
	char* rc = trace_off.strncat(dest, src, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "strncat",
			.res_size = n,
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static void trace_bcopy(const void *src, void *dest, size_t n)
{
	trace_off.bcopy(src, dest, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "bcopy",
			.res_size = n,
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
}

static void trace_bzero(void *s, size_t n)
{
	trace_off.bzero(s, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "bzero",
			.res_size = n,
			.res_id = (pointer_t)s,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
}

static char* trace_strdup(const char *s)
{
	char* rc = trace_off.strdup(s);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "strdup",
			.res_size = strlen(s),
			.res_id = (pointer_t)s,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static char* trace_strndup(const char *s, size_t n)
{
	char* rc = trace_off.strndup(s, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "strndup",
			.res_size = n,
			.res_id = (pointer_t)s,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

#ifndef strdupa
static char* trace_strdupa(const char *s)
{
	char* rc = trace_off.strdupa(s);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "strdupa",
			.res_size = strlen(s),
			.res_id = (pointer_t)s,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}
#endif

#ifndef strndupa
static char* trace_strndupa(const char *s, size_t n)
{
	char* rc = trace_off.strndupa(s, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "strndupa",
			.res_size = n,
			.res_id = (pointer_t)s,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}
#endif

static wchar_t* trace_wmemcpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	wchar_t* rc = trace_off.wmemcpy(dest, src, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "wmemcpy",
			.res_size = n * sizeof(wchar_t),
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static wchar_t* trace_wmempcpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	wchar_t* rc = trace_off.wmempcpy(dest, src, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "wmempcpy",
			.res_size = n * sizeof(wchar_t),
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static wchar_t* trace_wmemmove(wchar_t* dest, const wchar_t* src, size_t n)
{
	wchar_t* rc = trace_off.wmemmove(dest, src, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "wmemmove",
			.res_size = n * sizeof(wchar_t),
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static wchar_t* trace_wmemset(wchar_t *s, wchar_t c, size_t n)
{
	wchar_t* rc = trace_off.wmemset(s, c, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "wmemset",
			.res_size = n * sizeof(wchar_t),
			.res_id = (pointer_t)s,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static wchar_t* trace_wcscpy(wchar_t *dest, const wchar_t *src)
{
	wchar_t* rc = trace_off.wcscpy(dest, src);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "wcscpy",
			.res_size =  wcslen(src) * sizeof(wchar_t),
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static wchar_t* trace_wcsncpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	wchar_t* rc = trace_off.wcsncpy(dest, src, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "wcsncpy",
			.res_size =  n * sizeof(wchar_t),
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static wchar_t* trace_wcpcpy(wchar_t *dest, const wchar_t *src)
{
	wchar_t* rc = trace_off.wcpcpy(dest, src);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "wcpcpy",
			.res_size =  wcslen(src) * sizeof(wchar_t),
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static wchar_t* trace_wcpncpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	wchar_t* rc = trace_off.wcpncpy(dest, src, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "wcpncpy",
			.res_size =  n * sizeof(wchar_t),
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static wchar_t* trace_wcscat(wchar_t *dest, const wchar_t *src)
{
	wchar_t* rc = trace_off.wcscat(dest, src);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "wcscat",
			.res_size =  wcslen(src) * sizeof(wchar_t),
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static wchar_t* trace_wcsncat(wchar_t *dest, const wchar_t *src, size_t n)
{
	wchar_t* rc = trace_off.wcsncat(dest, src, n);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "wcsncat",
			.res_size =  n * sizeof(wchar_t),
			.res_id = (pointer_t)src,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}

static wchar_t* trace_wcsdup(const wchar_t *s)
{
	wchar_t* rc = trace_off.wcsdup(s);
	module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_memtransfer.id,
			.name = "wcsdup",
			.res_size =   wcslen(s) * sizeof(wchar_t),
			.res_id = (pointer_t)s,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
	return rc;
}


static trace_t trace_on = {
	.strcpy = trace_strcpy,
	.mempcpy = trace_mempcpy,
	.memmove = trace_memmove,
	.memcpy = trace_memcpy,
	.memset = trace_memset,
	.strncpy = trace_strncpy,
	.stpcpy = trace_stpcpy,
	.strcat = trace_strcat,
	.strncat = trace_strncat,
	.bcopy = trace_bcopy,
	.bzero = trace_bzero,
	.strdup = trace_strdup,
	.strndup = trace_strndup,
#ifndef strdupa
	.strdupa = trace_strdupa,
#endif
#ifndef strndupa
	.strndupa = trace_strndupa,
#endif
	.wmemcpy = trace_wmemcpy,
	.wmempcpy = trace_wmempcpy,
	.wmemmove = trace_wmemmove,
	.wmemset = trace_wmemset,
	.wcscpy = trace_wcscpy,
	.wcsncpy = trace_wcsncpy,
	.wcpcpy = trace_wcpcpy,
	.wcpncpy = trace_wcpncpy,
	.wcscat = trace_wcscat,
	.wcsncat = trace_wcsncat,
	.wcsdup = trace_wcsdup,
};

/*
 * Target functions.
 */


char* strcpy(char* dst, const char* src)
{
	return trace_rt->strcpy(dst, src);
}

void* mempcpy(void *dest, const void *src, size_t n)
{
	return trace_rt->mempcpy(dest, src, n);
}

void* memmove(void *dest, const void *src, size_t n)
{
	return trace_rt->memmove(dest, src, n);
}

void* memcpy(void *dest, const void *src, size_t n)
{
	return trace_rt->memcpy(dest, src, n);
}

void* memset(void *s, int c, size_t n)
{
	return trace_rt->memset(s, c, n);
}


char* strncpy(char *dest, const char *src, size_t n)
{
	return trace_rt->strncpy(dest, src, n);
}

char* stpcpy(char *dest, const char *src)
{
	return trace_rt->stpcpy(dest, src);
}

char* strcat(char *dest, const char *src)
{
	return trace_rt->strcat(dest, src);
}

char* strncat(char *dest, const char *src, size_t n)
{
	return trace_rt->strncat(dest, src, n);
}

void bcopy(const void *src, void *dest, size_t n)
{
	return trace_rt->bcopy(src, dest, n);
}

void bzero(void *s, size_t n)
{
	return trace_rt->bzero(s, n);
}

char* strdup(const char *s)
{
	return trace_rt->strdup(s);
}

char* strndup(const char *s, size_t n)
{
	return trace_rt->strndup(s, n);
}

#ifndef strdupa
char* strdupa(const char *s)
{
	return trace_rt->strdupa(s);
}
#endif
#ifndef strndupa
char* strndupa(const char *s, size_t n)
{
	return trace_rt->strndupa(s, n);
}
#endif

wchar_t* wmemcpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	return trace_rt->wmemcpy(dest, src, n);
}

wchar_t* wmempcpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	return trace_rt->wmempcpy(dest, src, n);
}

wchar_t* wmemmove(wchar_t* dest, const wchar_t* src, size_t b)
{
	return trace_rt->wmemmove(dest, src, b);
}

wchar_t* wmemset(wchar_t *s, wchar_t c, size_t n)
{
	return trace_rt->wmemset(s, c, n);
}

wchar_t* wcscpy(wchar_t *dest, const wchar_t *src)
{
	return trace_rt->wcscpy(dest, src);
}

wchar_t* wcsncpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	return trace_rt->wcsncpy(dest, src, n);
}

wchar_t* wcpcpy(wchar_t *dest, const wchar_t *src)
{
	return trace_rt->wcpcpy(dest, src);
}

wchar_t* wcpncpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	return trace_rt->wcpncpy(dest, src, n);
}

wchar_t* wcscat(wchar_t *dest, const wchar_t *src)
{
	return trace_rt->wcscat(dest, src);
}

wchar_t* wcsncat(wchar_t *dest, const wchar_t *src, size_t n)
{
	return trace_rt->wcsncat(dest, src, n);
}

wchar_t* wcsdup(const wchar_t *s)
{
	return trace_rt->wcsdup(s);
}

/*
 * Initialization functions.
 */

static char* init_strcpy(char* dst, const char* src)
{
	trace_initialize();
	return trace_init_rt->strcpy(dst, src);
}

static void* init_mempcpy(void *dest, const void *src, size_t n)
{
	trace_initialize();
	return trace_init_rt->mempcpy(dest, src, n);
}

static void* init_memmove(void *dest, const void *src, size_t n)
{
	trace_initialize();
	return trace_init_rt->memmove(dest, src, n);
}

static void* init_memcpy(void *dest, const void *src, size_t n)
{
	trace_initialize();
	return trace_init_rt->memcpy(dest, src, n);
}

static void* init_memset(void *s, int c, size_t n)
{
	trace_initialize();
	return trace_init_rt->memset(s, c, n);
}

static char* init_strncpy(char *dest, const char *src, size_t n)
{
	trace_initialize();
	return trace_init_rt->strncpy(dest, src, n);
}

static char* init_stpcpy(char *dest, const char *src)
{
	trace_initialize();
	return trace_init_rt->stpcpy(dest, src);
}

static char* init_strcat(char *dest, const char *src)
{
	trace_initialize();
	return trace_init_rt->strcat(dest, src);
}

static char* init_strncat(char *dest, const char *src, size_t n)
{
	trace_initialize();
	return trace_init_rt->strncat(dest, src, n);
}

static void init_bcopy(const void *src, void *dest, size_t n)
{
	trace_initialize();
	return trace_init_rt->bcopy(src, dest, n);
}

static void init_bzero(void *s, size_t n)
{
	trace_initialize();
	return trace_init_rt->bzero(s, n);
}

static char* init_strdup(const char *s)
{
	trace_initialize();
	return trace_init_rt->strdup(s);
}

static char* init_strndup(const char *s, size_t n)
{
	trace_initialize();
	return trace_init_rt->strndup(s, n);
}

#ifndef strdupa
static char* init_strdupa(const char *s)
{
	trace_initialize();
	return trace_init_rt->strdupa(s);
}
#endif
#ifndef strndupa
static char* init_strndupa(const char *s, size_t n)
{
	trace_initialize();
	return trace_init_rt->strndupa(s, n);
}
#endif

static wchar_t* init_wmemcpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	trace_initialize();
	return trace_init_rt->wmemcpy(dest, src, n);
}

static wchar_t* init_wmempcpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	trace_initialize();
	return trace_init_rt->wmempcpy(dest, src, n);
}

static wchar_t* init_wmemmove(wchar_t* dest, const wchar_t* src, size_t b)
{
	trace_initialize();
	return trace_init_rt->wmemmove(dest, src, b);
}

static wchar_t* init_wmemset(wchar_t *s, wchar_t c, size_t n)
{
	trace_initialize();
	return trace_init_rt->wmemset(s, c, n);
}

static wchar_t* init_wcscpy(wchar_t *dest, const wchar_t *src)
{
	trace_initialize();
	return trace_init_rt->wcscpy(dest, src);
}

static wchar_t* init_wcsncpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	trace_initialize();
	return trace_init_rt->wcsncpy(dest, src, n);
}

static wchar_t* init_wcpcpy(wchar_t *dest, const wchar_t *src)
{
	trace_initialize();
	return trace_init_rt->wcpcpy(dest, src);
}

static wchar_t* init_wcpncpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	trace_initialize();
	return trace_init_rt->wcpncpy(dest, src, n);
}

static wchar_t* init_wcscat(wchar_t *dest, const wchar_t *src)
{
	trace_initialize();
	return trace_init_rt->wcscat(dest, src);
}

static wchar_t* init_wcsncat(wchar_t *dest, const wchar_t *src, size_t n)
{
	trace_initialize();
	return trace_init_rt->wcsncat(dest, src, n);
}

static wchar_t* init_wcsdup(const wchar_t *s)
{
	trace_initialize();
	return trace_init_rt->wcsdup(s);
}


static trace_t trace_init = {
	.strcpy = init_strcpy,
	.mempcpy = init_mempcpy,
	.memmove = init_memmove,
	.memcpy = init_memcpy,
	.memset = init_memset,
	.strncpy = init_strncpy,
	.stpcpy = init_stpcpy,
	.strcat = init_strcat,
	.strncat = init_strncat,
	.bcopy = init_bcopy,
	.bzero = init_bzero,
	.strdup = init_strdup,
	.strndup = init_strndup,
#ifndef strdupa
	.strdupa = init_strdupa,
#endif
#ifndef strndupa
	.strndupa = init_strndupa,
#endif
	.wmemcpy = init_wmemcpy,
	.wmempcpy = init_wmempcpy,
	.wmemmove = init_wmemmove,
	.wmemset = init_wmemset,
	.wcscpy = init_wcscpy,
	.wcsncpy = init_wcsncpy,
	.wcpcpy = init_wcpcpy,
	.wcpncpy = init_wcpncpy,
	.wcscat = init_wcscat,
	.wcsncat = init_wcsncat,
	.wcsdup = init_wcsdup,
};

/*
 *
 */

static void trace_memtransfer_init(void) __attribute__((constructor));
static void trace_memtransfer_fini(void) __attribute__((destructor));

static void trace_memtransfer_init(void)
{
	trace_initialize();
}

static void trace_memtransfer_fini(void)
{
	enable_tracing(false);
	LOG("fini");
}

/**
 * Gets module information data.
 *
 * @return  the module information data.
 */
const sp_rtrace_module_info_t* sp_rtrace_get_module_info()
{
	return &module_info;
}
