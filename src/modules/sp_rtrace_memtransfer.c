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
#include "config.h"

/**
 * @file sp_rtrace_memtransfer.c
 *
 * Memory transfer tracing module (libsp-rtrace-memtransfer.so) implementation.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <wchar.h>

#include "sp_rtrace_main.h"
#include "common/sp_rtrace_proto.h"

#ifdef strdup
 #undef strdup
#endif

#ifdef strndup
 #undef strndup
#endif

#ifdef strdupa
 #undef strdupa
#endif

#ifdef strndupa
 #undef strndupa
#endif

#ifdef strncpy
 #undef strncpy
#endif

#ifdef strncat
 #undef strncat
#endif


/* Module information */
static sp_rtrace_module_info_t module_info = {
		.type = MODULE_TYPE_PRELOAD,
		.version_major = 1,
		.version_minor = 0,
		.name = "memtransfer",
		.description = "Memory transfer tracing module. "
				       "Tracks calls of the functions that results in changing memory "
				       "blocks (strcpy, memmove, memset etc).",
};

/* the module identifier assigned by main module */
static int module_id = 0;


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
typedef char* (*strdupa_t)(const char *s);
typedef char* (*strndupa_t)(const char *s, size_t n);
typedef wchar_t* (*wmemcpy_t)(wchar_t *dest, const wchar_t *src, size_t n);
typedef wchar_t* (*wmempcpy_t)(wchar_t *dest, const wchar_t *src, size_t n);
typedef wchar_t* (*wmemmove_t)(wchar_t* dest, const wchar_t* src, size_t b);
typedef wchar_t* (*wmemset_t)(wchar_t *s, int c, size_t n);
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
	strdupa_t strdupa;
	strndupa_t strndupa;
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
static trace_t trace_off = {0};
/* tracing function references */
static trace_t trace_on;

static trace_t trace_init;

/* Runtime function references */
static trace_t* trace_rt = &trace_init;


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
static void trace_initialize()
{
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
	trace_off.strdupa = (strdupa_t)dlsym(RTLD_NEXT, "strdupa");
	trace_off.strndupa = (strndupa_t)dlsym(RTLD_NEXT, "strndupa");
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

	enable_tracing(false);
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
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "strcpy", strlen(src), (void*)src, NULL);
	return rc;
}

static void* trace_mempcpy(void *dest, const void *src, size_t n)
{
	void* rc = trace_off.mempcpy(dest, src, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "mempcpy", n, (void*)src, NULL);
	return rc;
}

static void* trace_memmove(void *dest, const void *src, size_t n)
{
	void* rc = trace_off.memmove(dest, src, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "memmove", n, (void*)src, NULL);
	return rc;
}

static void* trace_memcpy(void *dest, const void *src, size_t n)
{
	void* rc = trace_off.memcpy(dest, src, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "memcpy", n, (void*)src, NULL);
	return rc;
}

static void* trace_memset(void *s, int c, size_t n)
{
	void* rc = trace_off.memset(s, c, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "memset", n, (void*)s, NULL);
	return rc;
}

static char* trace_strncpy(char *dest, const char *src, size_t n)
{
	char* rc = trace_off.strncpy(dest, src, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "strncpy", n, (void*)src, NULL);
	return rc;
}

static char* trace_stpcpy(char *dest, const char *src)
{
	char* rc = trace_off.stpcpy(dest, src);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "stpcpy", strlen(src), (void*)src, NULL);
	return rc;
}

static char* trace_strcat(char *dest, const char *src)
{
	char* rc = trace_off.strcat(dest, src);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "strcat", strlen(src), (void*)src, NULL);
	return rc;
}

static char* trace_strncat(char *dest, const char *src, size_t n)
{
	char* rc = trace_off.strncat(dest, src, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "strncat", n, (void*)src, NULL);
	return rc;
}

static void trace_bcopy(const void *src, void *dest, size_t n)
{
	trace_off.bcopy(src, dest, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "bcopy", n, (void*)src, NULL);
}

static void trace_bzero(void *s, size_t n)
{
	trace_off.bzero(s, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "bzero", n, (void*)s, NULL);
}

static char* trace_strdup(const char *s)
{
	char* rc = trace_off.strdup(s);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "strdup", strlen(s), (void*)s, NULL);
	return rc;
}

static char* trace_strndup(const char *s, size_t n)
{
	char* rc = trace_off.strndup(s, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "strndup", n, (void*)s, NULL);
	return rc;
}

static char* trace_strdupa(const char *s)
{
	char* rc = trace_off.strdupa(s);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "strdupa", strlen(s), (void*)s, NULL);
	return rc;
}

static char* trace_strndupa(const char *s, size_t n)
{
	char* rc = trace_off.strndupa(s, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "strndupa", n, (void*)s, NULL);
	return rc;
}

static wchar_t* trace_wmemcpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	wchar_t* rc = trace_off.wmemcpy(dest, src, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "wmemcpy", n << 1, (void*)src, NULL);
	return rc;
}

static wchar_t* trace_wmempcpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	wchar_t* rc = trace_off.wmempcpy(dest, src, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "wmempcpy", n << 1, (void*)src, NULL);
	return rc;
}

static wchar_t* trace_wmemmove(wchar_t* dest, const wchar_t* src, size_t b)
{
	wchar_t* rc = trace_off.wmemmove(dest, src, b);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "wmemmove", b << 1, (void*)src, NULL);
	return rc;
}

static wchar_t* trace_wmemset(wchar_t *s, int c, size_t n)
{
	wchar_t* rc = trace_off.wmemset(s, c, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "wmemset", c << 1, (void*)s, NULL);
	return rc;
}

static wchar_t* trace_wcscpy(wchar_t *dest, const wchar_t *src)
{
	wchar_t* rc = trace_off.wcscpy(dest, src);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "wcscpy", wcslen(src) << 1, (void*)src, NULL);
	return rc;
}

static wchar_t* trace_wcsncpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	wchar_t* rc = trace_off.wcsncpy(dest, src, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "wcsncpy", n << 1, (void*)src, NULL);
	return rc;
}

static wchar_t* trace_wcpcpy(wchar_t *dest, const wchar_t *src)
{
	wchar_t* rc = trace_off.wcpcpy(dest, src);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "wcpcpy", wcslen(src) << 1, (void*)src, NULL);
	return rc;
}

static wchar_t* trace_wcpncpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	wchar_t* rc = trace_off.wcpncpy(dest, src, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "wcpncpy", n << 1, (void*)src, NULL);
	return rc;
}

static wchar_t* trace_wcscat(wchar_t *dest, const wchar_t *src)
{
	wchar_t* rc = trace_off.wcscat(dest, src);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "wcscat", wcslen(src) << 1, (void*)src, NULL);
	return rc;
}

static wchar_t* trace_wcsncat(wchar_t *dest, const wchar_t *src, size_t n)
{
	wchar_t* rc = trace_off.wcsncat(dest, src, n);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "wcsncat", n << 1, (void*)src, NULL);
	return rc;
}

static wchar_t* trace_wcsdup(const wchar_t *s)
{
	wchar_t* rc = trace_off.wcsdup(s);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "wcsdup", wcslen(s) << 1, (void*)s, NULL);
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
	.strdupa = trace_strdupa,
	.strndupa = trace_strndupa,
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

char* strdupa(const char *s)
{
	return trace_rt->strdupa(s);
}

char* strndupa(const char *s, size_t n)
{
	return trace_rt->strndupa(s, n);
}

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
	return trace_off.strcpy(dst, src);
}


static void* init_mempcpy(void *dest, const void *src, size_t n)
{
	trace_initialize();
	return trace_off.mempcpy(dest, src, n);
}

static void* init_memmove(void *dest, const void *src, size_t n)
{
	trace_initialize();
	return trace_off.memmove(dest, src, n);
}

static void* init_memcpy(void *dest, const void *src, size_t n)
{
	trace_initialize();
	return trace_off.memcpy(dest, src, n);
}

static void* init_memset(void *s, int c, size_t n)
{
	trace_initialize();
	return trace_off.memset(s, c, n);
}

static char* init_strncpy(char *dest, const char *src, size_t n)
{
	trace_initialize();
	return trace_off.strncpy(dest, src, n);
}

static char* init_stpcpy(char *dest, const char *src)
{
	trace_initialize();
	return trace_off.stpcpy(dest, src);
}

static char* init_strcat(char *dest, const char *src)
{
	trace_initialize();
	return trace_off.strcat(dest, src);
}

static char* init_strncat(char *dest, const char *src, size_t n)
{
	trace_initialize();
	return trace_off.strncat(dest, src, n);
}

static void init_bcopy(const void *src, void *dest, size_t n)
{
	trace_initialize();
	return trace_off.bcopy(src, dest, n);
}

static void init_bzero(void *s, size_t n)
{
	trace_initialize();
	return trace_off.bzero(s, n);
}

static char* init_strdup(const char *s)
{
	trace_initialize();
	return trace_off.strdup(s);
}

static char* init_strndup(const char *s, size_t n)
{
	trace_initialize();
	return trace_off.strndup(s, n);
}

static char* init_strdupa(const char *s)
{
	trace_initialize();
	return trace_off.strdupa(s);
}

static char* init_strndupa(const char *s, size_t n)
{
	trace_initialize();
	return trace_off.strndupa(s, n);
}

static wchar_t* init_wmemcpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	trace_initialize();
	return trace_off.wmemcpy(dest, src, n);
}

static wchar_t* init_wmempcpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	trace_initialize();
	return trace_off.wmempcpy(dest, src, n);
}

static wchar_t* init_wmemmove(wchar_t* dest, const wchar_t* src, size_t b)
{
	trace_initialize();
	return trace_off.wmemmove(dest, src, b);
}

static wchar_t* init_wmemset(wchar_t *s, wchar_t c, size_t n)
{
	trace_initialize();
	return trace_off.wmemset(s, c, n);
}

static wchar_t* init_wcscpy(wchar_t *dest, const wchar_t *src)
{
	trace_initialize();
	return trace_off.wcscpy(dest, src);
}

static wchar_t* init_wcsncpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	trace_initialize();
	return trace_off.wcsncpy(dest, src, n);
}

static wchar_t* init_wcpcpy(wchar_t *dest, const wchar_t *src)
{
	trace_initialize();
	return trace_off.wcpcpy(dest, src);
}

static wchar_t* init_wcpncpy(wchar_t *dest, const wchar_t *src, size_t n)
{
	trace_initialize();
	return trace_off.wcpncpy(dest, src, n);
}

static wchar_t* init_wcscat(wchar_t *dest, const wchar_t *src)
{
	trace_initialize();
	return trace_off.wcscat(dest, src);
}

static wchar_t* init_wcsncat(wchar_t *dest, const wchar_t *src, size_t n)
{
	trace_initialize();
	return trace_off.wcsncat(dest, src, n);
}

static wchar_t* init_wcsdup(const wchar_t *s)
{
	trace_initialize();
	return trace_off.wcsdup(s);
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
	.strdupa = init_strdupa,
	.strndupa = init_strndupa,
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

static void trace_memory_init(void) __attribute__((constructor));
static void trace_memory_fini(void) __attribute__((destructor));

static void trace_memory_init(void)
{
	trace_initialize();
	module_id = sp_rtrace_register_module(module_info.name, module_info.version_major, module_info.version_minor, enable_tracing);
}

static void trace_memory_fini(void)
{
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
