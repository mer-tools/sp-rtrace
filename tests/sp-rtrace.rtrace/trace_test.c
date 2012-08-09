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
 * @file trace_test.c
 *
 * This file contains loop of memory allocations/deallocations
 * used to test various process tracing scenarios.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void zero(int size)
{
	free(malloc(size));
}


void one(int size)
{
	zero(size);
}


void two(int size)
{
	one(size);
}

void three(int size)
{
	two(size);
}

void four(int size)
{
	three(size);
}

void five(int size)
{
	four(size);
}

static sig_atomic_t s_loop = 1;

static void sigint_handler(int sig)
{
	s_loop = 0;
}

void start(void)
{
	int size = 1000;
	while (s_loop) {
		five(size++);
		sleep(1);
	}
}

int main()
{
	/* install interrupt handler */
	struct sigaction sa = {.sa_flags = 0, .sa_handler = sigint_handler};
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGUSR2, &sa, NULL) == -1) {
		fprintf(stderr, "ERROR: Failed to install SIGUSR2 handler\n");
		return -1;
	}
	start();
	return 0;
}
