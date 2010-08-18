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
/**
 * @file context_test.c
 *
 * Test application for libs-rtrace1 context API with preloaded
 * libsp-rtrace-main.so library.
 */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>

#include "rtrace_testsuite.h"
#include "library/sp_rtrace_context.h"

RT_INIT();

RT_CASE(context)
{
	int* context_mask = (int*)dlsym(RTLD_DEFAULT, "sp_context_mask");
	RT_ASSERT(context_mask != NULL);

	int context1 = sp_rtrace_context_try_create("context1");
	RT_ASSERT(context1 != 0);
	int context2 = sp_rtrace_context_try_create("context2");
	RT_ASSERT(context2 != 0);

	sp_rtrace_context_try_enter(context1);
	RT_ASSERT(*context_mask == context1);
	sp_rtrace_context_try_enter(context2);
	RT_ASSERT(*context_mask == context1 | context2);
	sp_rtrace_context_try_exit(context1);
	RT_ASSERT(*context_mask == context2);
	sp_rtrace_context_try_exit(context2);
	RT_ASSERT(*context_mask == 0);

	return RT_OK;
}

int main()
{
	RT_START("context");
	RT_RUN_CASE_NO_MEMCHECK(context);

	return 0;
}

