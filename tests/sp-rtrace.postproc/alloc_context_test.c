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
 * @file alloc_context_test.c
 *
 * This file contains few memory allocation/deallocation calls for
 * basic resource trace testing with contexts.
 */


#include <stdlib.h>
#include "library/sp_rtrace_context.h"

int context1 = 0;
int context2 = 0;

static void* zero()
{
	if (malloc(1000) == NULL) {
		exit (-1);
	}
	return malloc(2000);
}

void*  one()
{
	sp_rtrace_context_try_enter(context1);
	if (malloc(1001) == NULL) {
		exit (-1);
	}
	free(zero());
	sp_rtrace_context_try_exit(context1);
	return malloc(2001);
}

void* two()
{
	sp_rtrace_context_try_enter(context2);
	if (malloc(1002) == NULL) {
		exit (-1);
	}
	free(one());
	sp_rtrace_context_try_exit(context2);
	return malloc(2002);
}

void* three()
{
	if (malloc(1003) == NULL) {
		exit (-1);
	}
	free(two());
	return malloc(2003);
}

void start()
{
	context1 = sp_rtrace_context_try_create("first context");
	context2 = sp_rtrace_context_try_create("second context");

	int i;
	for (i = 0; i < 5; i++) {
		free(three());
	}
	for (i = 0; i < 3; i++) {
		free(two());
	}
}

int main()
{
	start();
	sleep(1);
	return 0;
}
