/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010,2011 by Nokia Corporation
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
#include <limits.h>

#include "sp_rtrace_defs.h"
#include "sp_rtrace_formatter.h"
#include "common/sp_rtrace_proto.h"
#include "common/header.h"
#include "common/rtrace_data.h"

const char* sp_rtrace_resource_flags_text[SP_RTRACE_RESOURCE_FLAGS_MAX] = {
		"refcount",
};


const char* header_fields[SP_RTRACE_HEADER_MAX] = {
		"version",
		"arch",
		"timestamp",
		"process",
		"pid",
		"filter",
		"backtrace depth",
		"origin",
};

int sp_rtrace_print_header(FILE* fp, const struct sp_rtrace_header_t* header)
{
	char timestamp_s[32], version_s[32];
	if (!header->fields[SP_RTRACE_HEADER_TIMESTAMP]) {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		struct tm* tm = localtime(&tv.tv_sec);
		sprintf(timestamp_s, "%02d.%02d.%04d %02d:%02d:%02d", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900,
				tm->tm_hour, tm->tm_min, tm->tm_sec);
	}
	snprintf(version_s, sizeof(version_s), "%d.%d", SP_RTRACE_PROTO_VERSION_MAJOR, SP_RTRACE_PROTO_VERSION_MINOR);

	char buffer[PATH_MAX];
	int size = 0, i;
	for (i = 0; i < SP_RTRACE_HEADER_MAX; i++) {
		const char* value = header->fields[i];
		switch (i) {
			case SP_RTRACE_HEADER_TIMESTAMP:
				/* fill in timestamp value if the timestamp field is NULL */
				if (!value) value = timestamp_s;
				break;
			case SP_RTRACE_HEADER_VERSION:
				/* override the version information with the predefined version number */
				value = version_s;
				break;
		}
		if (value) {
			size += snprintf(buffer + size, PATH_MAX - size, "%s=%s, ", header_fields[i], value);
		}
	}
	buffer[size++] = '\n';
	buffer[size] = '\0';
	if (fputs(buffer, fp) == EOF) return -errno;
	return 0;
}


int sp_rtrace_print_mmap(FILE* fp, const struct sp_rtrace_mmap_t* mmap)
{
	if (fprintf(fp, ": %s => 0x%lx-0x%lx\n", mmap->module, mmap->from, mmap->to) == 0) return -errno;
	return 0;
}


int sp_rtrace_print_call(FILE* fp, const struct sp_rtrace_fcall_t* call)
{
	char buffer[2048], *ptr = buffer;

	ptr += sprintf(ptr, "%d. ", call->index);
	if (call->context) {
		ptr += sprintf(ptr, "@%x ", (int)call->context);
	}
	unsigned int timestamp = call->timestamp;
	if (timestamp == (unsigned int)-1) {
		struct timespec ts;
		if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
			timestamp = ts.tv_nsec / 1000000 + ts.tv_sec % (60 * 60 * 24) * 1000;
		}
	}
	if (timestamp) {
		int hours = timestamp / (1000 * 60 * 60);
		int usecs = timestamp % (1000 * 60 * 60);
		int minutes = usecs / (1000 * 60);
		usecs %= 1000 * 60;
		int seconds = usecs / 1000;
		usecs %= 1000;
		ptr += sprintf(ptr, "[%02d:%02d:%02d.%03d] ", hours, minutes, seconds, usecs);
	}
	ptr += sprintf(ptr, "%s", call->name);

	/* append resource type for multi resource traces */
	char* res_name = NULL;
	switch (call->res_type_flag) {
		case SP_RTRACE_FCALL_RFIELD_REF: {
			rd_resource_t* res = call->res_type;
			if (res && !res->hide) {
				res_name =  res->data.type;
			}
			break;
		}
		case SP_RTRACE_FCALL_RFIELD_NAME: {
			res_name = call->res_type;
			break;
		}
	}
	if (res_name) {
		ptr += sprintf(ptr, "<%s>", res_name);
	}

	if (call->type == SP_RTRACE_FTYPE_ALLOC) {
		ptr += sprintf(ptr, "(%d) = 0x%lx", call->res_size, call->res_id);
	}
	else {
		ptr += sprintf(ptr, "(0x%lx)", call->res_id);
	}
	*ptr++ = '\n';
	if (fwrite(buffer, 1, ptr - buffer, fp) < (size_t)(ptr - buffer)) return -errno;
	return 0;
}


int sp_rtrace_print_trace(FILE* fp, const struct sp_rtrace_ftrace_t* trace)
{
	int i, size = trace->nframes;
	char buffer[PATH_MAX];
	for (i = 0; i < size; i++) {
		char* ptr = buffer;
		ptr += sprintf(ptr, "\t0x%lx", trace->frames[i]);
		if (trace->resolved_names && trace->resolved_names[i]) {
			ptr += sprintf(ptr, " %s", trace->resolved_names[i]);
		}
		*ptr++ = '\n';
		if (fwrite(buffer, 1, ptr - buffer, fp) < (size_t)(ptr - buffer)) return -errno;
	}
	if (fputc('\n', fp) == EOF) return -errno;
	return 0;
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


int sp_rtrace_print_context(FILE* fp, const struct sp_rtrace_context_t* context)
{
	if (fprintf(fp, "@ %x : %s\n", (int)context->id, context->name) == 0) return -errno;
	return 0;
}


int sp_rtrace_print_resource(FILE* fp, const struct sp_rtrace_resource_t* resource)
{
	char buffer[PATH_MAX], *ptr = buffer;
	ptr += sprintf(ptr, "<%x> : %s (%s)", 1 << ((int)resource->id - 1), resource->type, resource->desc);
	if (resource->flags) {
		*ptr++ = ' ';
		*ptr++ = '[';
		unsigned int nflag = 0, flag;
		while ( (flag = 1 << nflag) <= SP_RTRACE_RESOURCE_LAST_FLAG) {
			if (nflag) *ptr++ = '|';
			ptr += sprintf(ptr, "%s", sp_rtrace_resource_flags_text[nflag++]);
		}
		*ptr++ = ']';
	}
	*ptr++ = '\n';
	*ptr = '\0';
	if (fwrite(buffer, 1, ptr - buffer, fp) < (size_t)(ptr - buffer)) return -errno;
	return 0;
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


int sp_rtrace_print_args(FILE* fp, const struct sp_rtrace_farg_t* args)
{
	while (args->name) {
		if (fprintf(fp, "\t$%s = %s\n", args->name, args->value) == 0) return -errno;
		args++;
	}
	return 0;
}


int sp_rtrace_print_attachment(FILE* fp, const struct sp_rtrace_attachment_t* file)
{
	if (fprintf(fp, "& %s : %s\n", file->name, file->path) == 0) return -errno;
	return 0;
}
