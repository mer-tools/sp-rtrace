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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "library/sp_rtrace_formatter.h"
#include "common/utils.h"

#define PRINT_HEADER 		"header"
#define PRINT_MMAP			"mmap"
#define PRINT_CALL			"call"
#define PRINT_TRACE			"trace"
#define PRINT_TRACE_STEP	"trace_step"
#define PRINT_CONTEXT		"context"
#define PRINT_COMMENT		"comment"
#define PRINT_ARGS			"args"

enum {
	HEADER_VERSION,
	HEADER_ARCH,
	HEADER_TIMESTAMP,
	HEADER_PID,
	HEADER_NAME,
};

enum {
	MMAP_MODULE,
	MMAP_FROM,
	MMAP_TO,
};

enum {
	CALL_INDEX,
	CALL_CONTEXT,
	CALL_TIMESTAMP,
	CALL_NAME,
	CALL_RES_SIZE,
	CALL_RES_ID,
};

enum {
	TRACE_FRAMES,
	TRACE_ADDRESS = 0,
	TRACE_RESOLVED = 1,
};

enum {
	CONTEXT_ID,
	CONTEXT_NAME,
};

/**
 * Prints header record.
 *
 * Arguments
 *   $1 - version
 *   $2 - architecture
 *   $3 - timestamp
 *   $4 - pid
 *   $5 - process name
 * @param argv
 */
static void print_header(char* argv[])
{
	int pid = atoi(argv[HEADER_PID]);
	struct tm tm = {.tm_isdst = daylight};
	struct timeval tv = {0};

	sscanf(argv[HEADER_TIMESTAMP], "%d.%d.%d %d:%d:%d", &tm.tm_mday, &tm.tm_mon, &tm.tm_year,
			&tm.tm_hour, &tm.tm_min, &tm.tm_sec);
	tm.tm_mon--;
	tm.tm_year -= 1900;
	tv.tv_sec = timelocal(&tm);

	sp_rtrace_print_header(stdout, argv[HEADER_VERSION], argv[HEADER_ARCH], &tv, pid, argv[HEADER_NAME]);
}

/**
 * Prints memory mapping record.
 *
 * Arguments:
 *   $1 - module path
 *   $2 - starting address
 *   $3 - ending address
 * @param argv
 */
static void print_mmap(char* argv[])
{
	void *from = NULL, *to = NULL;
	sscanf(argv[MMAP_FROM], "%p", &from);
	sscanf(argv[MMAP_TO], "%p", &to);

	sp_rtrace_print_mmap(stdout, argv[MMAP_MODULE], from, to);
}

/**
 * Prints function call record.
 *
 * Arguments:
 *   $1 - index
 *   $2 - context (can be empty string)
 *   $3 - timestamp (can be empty string)
 *   $4 - function name
 *   $5 - resource size(allocation)/id(deallocation)
 *   $6 - resource id, empty string for deallocation records
 * @param argv
 */
static void print_call(char* argv[])
{
	int index = atoi(argv[CALL_INDEX]);
	int context = 0;
	sscanf(argv[CALL_CONTEXT], "%x", &context);
	int hours = 0, minutes = 0, seconds = 0, msecs = 0;
	sscanf(argv[CALL_TIMESTAMP], "%d:%d:%d.%d", &hours, &minutes, &seconds, &msecs);
	int timestamp = hours * 60 * 60 * 1000 + minutes * 60 * 1000 + seconds * 1000 + msecs;
	int res_size = atoi(argv[CALL_RES_SIZE]);
	void* res_id = NULL;
	sscanf(argv[CALL_RES_ID], "%p", &res_id);

	sp_rtrace_print_call(stdout, index, context, timestamp, argv[CALL_NAME], res_size, res_id);
}

/**
 * Prints function trace record.
 *
 * Arguments:
 *   $1 - number of backtrace frames
 *   $2 - address
 *   $3 - resolved name (can be empty string)
 *   $4 - address
 *   $5 - resolved name (can be empty string)
 *   ...
 *
 * @param argv
 */
static void print_trace(char* argv[])
{
	int nframes = atoi(argv[TRACE_FRAMES]);
	int i;
	void** frames = malloc_a(sizeof(void*) * nframes);
	char** resolved = calloc(nframes, sizeof(char*));
	char** parg = &argv[TRACE_FRAMES + 1];
	bool is_resolved = false;
	for (i = 0; i < nframes; i++) {
		sscanf(*parg++, "%p", &frames[i]);
		if (**parg) {
			resolved[i] = strdup_a(*parg);
			is_resolved = true;
		}
		parg++;
	}
	sp_rtrace_print_trace(stdout, frames, is_resolved ? resolved : NULL, nframes);

	for (i = 0; i < nframes; i++) {
		if (resolved[i]) free(resolved[i]);
	}
	free(frames);
	free(resolved);
}


/**
 * Prints function trace step.
 *
 * Arguments:
 *   $1 - address
 *   $2 - resolved name (can be empty string)
 *
 * @param argv
 */
static void print_trace_step(char* argv[])
{
	void* addr = NULL;
	sscanf(argv[TRACE_ADDRESS], "%p", &addr);
	sp_rtrace_print_trace_step(stdout, addr, *argv[TRACE_RESOLVED] ? argv[TRACE_RESOLVED] : NULL);
}

/**
 * Prints context registry record
 *
 * Arguments:
 *   $1 - context id
 *   $2 - contex name
 *
 * @param argv
 */
static void print_context(char* argv[])
{
	int context_id = 0;
	sscanf(argv[CONTEXT_ID], "%x", &context_id);
	sp_rtrace_print_context(stdout, context_id, argv[CONTEXT_NAME]);
}


/**
 * Prints comment record
 *
 * Arguments:
 *   $1 - comment
 *
 * @param argv
 */
static void print_comment(char* argv[])
{
	sp_rtrace_print_comment(stdout, "%s\n", argv[0]);
}


/**
 * Prints function arguments record
 *
 * Arguments:
 *   $1 - first argument
 *   $2 - second argument
 *   ...
 *   $n - NULL
 *
 * @param argv
 */
static void print_args(char* argv[])
{
	sp_rtrace_print_args(stdout, argv);
}


/**
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[])
{
	tzset();
	if (argc > 1) {
		if (!strcmp(argv[1], PRINT_HEADER)) {
			print_header(&argv[2]);
		}
		else if (!strcmp(argv[1], PRINT_MMAP)) {
			print_mmap(&argv[2]);
		}
		else if (!strcmp(argv[1], PRINT_CALL)) {
			print_call(&argv[2]);
		}
		else if (!strcmp(argv[1], PRINT_TRACE)) {
			print_trace(&argv[2]);
		}
		else if (!strcmp(argv[1], PRINT_TRACE_STEP)) {
			print_trace_step(&argv[2]);
		}
		else if (!strcmp(argv[1], PRINT_CONTEXT)) {
			print_context(&argv[2]);
		}
		else if (!strcmp(argv[1], PRINT_COMMENT)) {
			print_comment(&argv[2]);
		}
		else if (!strcmp(argv[1], PRINT_ARGS)) {
			print_args(&argv[2]);
		}
	}
	return 0;
}


