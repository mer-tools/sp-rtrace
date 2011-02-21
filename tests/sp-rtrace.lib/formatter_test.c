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
#include "library/sp_rtrace_defs.h"

#define PRINT_HEADER 		"header"
#define PRINT_MMAP			"mmap"
#define PRINT_CALL			"call"
#define PRINT_TRACE			"trace"
#define PRINT_TRACE_STEP	"trace_step"
#define PRINT_CONTEXT		"context"
#define PRINT_COMMENT		"comment"
#define PRINT_ARGS			"args"
#define PRINT_RESOURCE		"resource"

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
	CALL_RES_TYPE,
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

enum {
	RESOURCE_ID,
	RESOURCE_TYPE,
	RESOURCE_DESC,
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
	sp_rtrace_header_t header = {
			.fields = {
					argv[HEADER_VERSION],
					argv[HEADER_ARCH],
					argv[HEADER_TIMESTAMP],
					argv[HEADER_NAME],
					argv[HEADER_PID],
					NULL,
					NULL,
			}
	};
	sp_rtrace_print_header(stdout, &header);
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
	sp_rtrace_mmap_t mmap = {
			.module = argv[MMAP_MODULE],
	};

	sscanf(argv[MMAP_FROM], "%lx", &mmap.from);
	sscanf(argv[MMAP_TO], "%lx", &mmap.to);

	sp_rtrace_print_mmap(stdout, &mmap);
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
 *   $7 - resource type name, empty string if the resource type should be omitted.
 * @param argv
 */
static void print_call(char* argv[])
{
	sp_rtrace_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.index = atoi(argv[CALL_INDEX]),
			.res_size = atoi(argv[CALL_RES_SIZE]),
			.res_type = *argv[CALL_RES_TYPE] ? argv[CALL_RES_TYPE] : NULL,
			.res_type_flag = SP_RTRACE_FCALL_RFIELD_NAME,
			.name = argv[CALL_NAME],
	};
	if (!call.res_size) call.type = SP_RTRACE_FTYPE_FREE;

	sscanf(argv[CALL_CONTEXT], "%x", &call.context);

	int hours = 0, minutes = 0, seconds = 0, msecs = 0;
	sscanf(argv[CALL_TIMESTAMP], "%d:%d:%d.%d", &hours, &minutes, &seconds, &msecs);
	call.timestamp = hours * 60 * 60 * 1000 + minutes * 60 * 1000 + seconds * 1000 + msecs;

	sscanf(argv[CALL_RES_ID], "%lx", &call.res_id);

	sp_rtrace_print_call(stdout, &call);
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
	sp_rtrace_ftrace_t trace = {
		.nframes = atoi(argv[TRACE_FRAMES])
	};
	int i;
	trace.frames = malloc_a(sizeof(void*) * trace.nframes);
	trace.resolved_names = calloc(trace.nframes, sizeof(char*));
	char** parg = &argv[TRACE_FRAMES + 1];
	bool is_resolved = false;
	for (i = 0; i < trace.nframes; i++) {
		sscanf(*parg++, "%lx", &trace.frames[i]);
		if (**parg) {
			trace.resolved_names[i] = strdup_a(*parg);
			is_resolved = true;
		}
		parg++;
	}
	sp_rtrace_print_trace(stdout, &trace);

	for (i = 0; i < trace.nframes; i++) {
		if (trace.resolved_names[i]) free(trace.resolved_names[i]);
	}
	free(trace.frames);
	free(trace.resolved_names);
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
 *   $2 - context name
 *
 * @param argv
 */
static void print_context(char* argv[])
{
	sp_rtrace_context_t context = {
			.name = argv[CONTEXT_NAME],
	};
	sscanf(argv[CONTEXT_ID], "%x", &context.id);
	sp_rtrace_print_context(stdout, &context);
}

/**
 * Prints resource registry record
 *
 * Arguments:
 *   $1 - resource id
 *   $2 - resource type
 *   $3 - resource description
 *
 * @param argv
 */
static void print_resource(char* argv[])
{
	sp_rtrace_resource_t resource = {
			.type = argv[RESOURCE_TYPE],
			.desc = argv[RESOURCE_DESC],
	};
	sscanf(argv[RESOURCE_ID], "%x", &resource.id);
	sp_rtrace_print_resource(stdout, &resource);
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
	sp_rtrace_farg_t args[32], *parg = args;
	while (**argv) {
		char* ptr = strchr(*argv, '=');
		if (ptr) {
			*ptr++ = '\0';
			parg->name = *argv;
			parg->value = ptr;
		}
		argv++;
	}
	parg->name = NULL;
	parg->value = NULL;
	sp_rtrace_print_args(stdout, args);
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
		else if (!strcmp(argv[1], PRINT_RESOURCE)) {
			print_resource(&argv[2]);
		}
	}
	return 0;
}


