/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2011 by Nokia Corporation
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
 * @file shmposix_test.c
 *
 * Test application for posix shared memory tracking module (shmposix) coverage.
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#define SEGMENT_NAME	"/shmposix"

int main(void)
{
	int fd = shm_open(SEGMENT_NAME, O_RDWR | O_CREAT, 0666);
	if (fd == -1) return -1;

	int size = 4096;

	void* ptr = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
	if (ptr != MAP_FAILED) {
		munmap(ptr, size);
	}

	ptr = mmap64(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
	if (ptr != MAP_FAILED) {
		munmap(ptr, size);
	}

	shm_unlink(SEGMENT_NAME);
    return 0;
}
