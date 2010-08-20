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

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "formatter.h"
#include "sp_rtrace_proto.h"

/*
 * rd_* data formatting function implementation
 */
int formatter_write_header(const formatter_header_t* header, FILE* fp)
{
	char buffer[2048];
	int size = 0;
	while (header->key) {
		size += sprintf(buffer + size,"%s=%s, ", header->key, header->value ? header->value : "");
		header++;
	}
	buffer[size++] = '\n';
	buffer[size] = '\0';
	fputs(buffer, fp);
	return 0;
}


int formatter_write_mmap(const rd_mmap_t* mmap, FILE* fp)
{
	fprintf(fp, ": %s => %p-%p\n", mmap->module, mmap->from, mmap->to);
	return 0;
}


int formatter_write_comment(const rd_comment_t* comment, FILE* fp)
{
	fputs(comment->text, fp);
	return 0;
}


int formatter_write_fcall(const rd_fcall_t*  call, FILE* fp)
{
	char buffer[1024], *ptr = buffer;

	ptr += sprintf(ptr, "%d. ", call->index);
	if (call->module_id) {
		ptr += sprintf(ptr, ":%x ", (int)call->module_id);
	}
	if (call->context) {
		ptr += sprintf(ptr, "@%x ", (int)call->context);
	}
	if (call->timestamp) {
		int hours = call->timestamp / (1000 * 60 * 60);
		int usecs = call->timestamp % (1000 * 60 * 60);
		int minutes = usecs / (1000 * 60);
		usecs %= 1000 * 60;
		int seconds = usecs / 1000;
		usecs %= 1000;
		ptr += sprintf(ptr, "[%02d:%02d:%02d.%03d] ", hours, minutes, seconds, usecs);
	}
	ptr += sprintf(ptr, "%s(", call->name);
	if (call->type == SP_RTRACE_FTYPE_ALLOC) {
		ptr += sprintf(ptr, "%d) = %p", call->res_size, call->res_id);
	}
	else {
		ptr += sprintf(ptr, "%p)", call->res_id);
	}
	*ptr++ = '\n';
	fwrite(buffer, 1, ptr - buffer, fp);
	return 0;
}

int formatter_write_ftrace(const rd_ftrace_t* trace, FILE* fp)
{
	int i, size = trace->nframes;
	char buffer[1024];
	for (i = 0; i < size; i++) {
		char* ptr = buffer;
		ptr += sprintf(ptr, "\t%p", trace->frames[i]);
		if (trace->resolved_names && trace->resolved_names[i]) {
			ptr += sprintf(ptr, " (%s)", trace->resolved_names[i]);
		}
		*ptr++ = '\n';
		fwrite(buffer, 1, ptr - buffer, fp);
	}
	return 0;
}


int formatter_write_context(const rd_context_t* context, FILE* fp)
{
	fprintf(fp, "@ %x : %s\n", (int)context->id, context->name);
	return 0;
}


int formatter_write_fargs(const rd_fargs_t* args, FILE* fp)
{
	char** ptr = args->args;
	int index = 1;

	while (*ptr) {
		fprintf(fp, "\t$%d = %s\n", index++, *ptr++);
	}
	return 0;
}

/*
 *
 */

int formatter_printf(FILE* fp, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(fp, format, args);
	va_end(args);
	return 0;
}
