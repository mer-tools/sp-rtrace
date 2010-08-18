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
 * @file memory_test.c
 *
 * Test application for memory module tracking coverage.
 */

#include <stdlib.h>

void test_memory()
{
	void* ptr = malloc(1024);
	free(ptr);

	ptr = calloc(1, 1024);
	ptr = realloc(ptr, 2048);
	free(ptr);

	posix_memalign(&ptr, 8, 1024);
}

int main()
{
	test_memory();
	return 0;
}
