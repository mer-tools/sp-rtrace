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
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <sys/time.h>
#include <errno.h>

#include "sp_rtrace_formatter.h"
#include "common/formatter.h"
#include "common/sp_rtrace_proto.h"

int sp_rtrace_print_header(FILE* fp, const char* version, const char* arch,
		const struct timeval *timestamp, int pid, const char* process_name)
{
	struct timeval tv;
	if (timestamp == NULL) {
		gettimeofday(&tv, NULL);
		timestamp = &tv;
	}

	char timestamp_s[32];
	struct tm* tm = localtime(&timestamp->tv_sec);
	sprintf(timestamp_s, "%02d.%02d.%04d %02d:%02d:%02d", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900,
			tm->tm_hour, tm->tm_min, tm->tm_sec);

	char pid_s[10];
	sprintf(pid_s, "%d", pid);

	const header_t header=  {
			.fields = { 
					(char*)version,       // HEADER_VERSION
					(char*)arch,          // HEADER_ARCH 
					timestamp_s,          // HEADER_TIMESTAMP
					(char*)process_name,  // HEADER_PROCESS
					pid_s,                // HEADER_PID
					NULL                  // HEADER_FILTER
			},
	};
	return formatter_write_header(&header, fp);
}


int sp_rtrace_print_mmap(FILE* fp, const char* module, void* from, void* to)
{
	const rd_mmap_t mmap = {.module = (char*)module, .from = (pointer_t)from, .to = (pointer_t)to};
	return formatter_write_mmap(&mmap, fp);
}


int sp_rtrace_print_call(FILE* fp, int index, unsigned int context, unsigned int timestamp,
		const char* name, int res_size, void* res_id, const char* res_type)
{
	rd_resource_t resource = {
			.type = (char*)res_type,
	};
	const rd_fcall_t call = {
			.index = index,
			.context = context,
			.timestamp = timestamp,
			.type = res_size ? SP_RTRACE_FTYPE_ALLOC : SP_RTRACE_FTYPE_FREE,
			.name = (char*)name,
			.res_id = (pointer_t)res_id,
			.res_size = res_size,
			.res_type = res_type ? &resource : NULL,
	};
	return formatter_write_fcall(&call, fp);
}


int sp_rtrace_print_trace(FILE* fp, void** frames, char** resolved, int nframes)
{
	const rd_ftrace_t trace = {.frames = (pointer_t*)frames, .nframes = nframes, .resolved_names = (char**)resolved};
	return formatter_write_ftrace(&trace, fp);
}


int sp_rtrace_print_trace_step(FILE* fp, void* addr, const char* resolved)
{
	int rc = 0;
	if (resolved) {
		rc = fprintf(fp, "\t0x%lx %s\n", (pointer_t)addr, resolved);
	}
	else {
		rc = fprintf(fp, "\t0x%lx\n", (pointer_t)addr);
	}
	if (rc == 0) {
		return -errno;
	}
	return 0;
}


int sp_rtrace_print_context(FILE* fp, unsigned int id, const char* name)
{
	const rd_context_t context = {.id = id, .name = (char*)name};
	return formatter_write_context(&context, fp);
}


int sp_rtrace_print_resource(FILE* fp, unsigned int id, const char* type, const char* desc)
{
	rd_resource_t resource = {.id = id, .type = (char*)type, .desc = (char*)desc};
	return formatter_write_resource(&resource, fp);
}



int sp_rtrace_print_comment(FILE* fp, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int rc = vfprintf(fp, format, args);
	va_end(args);
	if (rc == 0) {
		return -errno;
	}
	return 0;
}


int sp_rtrace_print_args(FILE* fp, char** args)
{
	const rd_fargs_t fargs = {.args = (char**)args};
	return formatter_write_fargs(&fargs, fp);
}


