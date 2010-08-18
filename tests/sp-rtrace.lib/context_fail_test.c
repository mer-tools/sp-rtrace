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
 * @file context_fail_test.c
 *
 * Test application for libs-rtrace1 context API without preloaded
 * libsp-rtrace-main.so library.
 */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>

#include "rtrace_testsuite.h"
#include "library/sp_rtrace_context.h"

RT_INIT();

RT_CASE(context_fail)
{
	int context1 = sp_rtrace_context_try_create("context1");
	RT_ASSERT(context1 == 0);

	return RT_OK;
}

int main()
{
	RT_START("context_fail");
	RT_RUN_CASE_NO_MEMCHECK(context_fail);

	return 0;
}

