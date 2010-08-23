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

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "writer.h"
#include "filter.h"
#include "leaks_sort.h"
#include "sp_rtrace_postproc.h"

#include "common/formatter.h"

/**
 * Writes module information log record into text log.
 *
 * @param[in] minfo   the module information data.
 * @param[in] formatter  the log file handle.
 * @return
 */
static int write_module_info(rd_minfo_t* minfo, FILE* fp)
{
	formatter_printf(fp, "# tracing module: [%x] %s (%d.%d)\n", minfo->id, minfo->name, minfo->vmajor, minfo->vminor);
	return 0;
}


/**
 * Range function for dlist_foreach2_in.
 *
 * @param[in] comment   the comment to check
 * @param[in] data      the current call index.
 * @return              true if the comment index is less than the current call index.
 */
static long comment_check_index(rd_comment_t* comment, void* data)
{
	long index = (long)data;
	return comment->index < index;
}

/**
 * Writes function call data into text log.
 *
 * This function writes full function call data including backtrace
 * and function arguments (TODO).
 * @param[in] call   the function call data.
 * @param[in] fmt    the formatter data.
 * @return
 */
static int write_function_call(rd_fcall_t* call, fmt_data_t* fmt)
{
	/* first write all comments with index less than current call index */
	if (fmt->comment) {
		fmt->comment = dlist_foreach2_in(&fmt->rd->comments, fmt->comment,
				(op_binary_t)comment_check_index, (void*)(long)call->index, (op_binary_t)formatter_write_comment, fmt->fp);
	}

	/* then write the function call itself */
	formatter_write_fcall(call, fmt->fp);
	if (call->args) formatter_write_fargs(call->args, fmt->fp);
	if (call->trace) formatter_write_ftrace(call->trace, fmt->fp);
	return 0;
}

/**
 * Writes compressed function call record into text log.
 *
 * This function writes only the function call record without
 * the backtrace. However arguments must still be written (TODO).
 * @param[in] call   the function call data.
 * @param[in] fmt    the formatter data.
 * @return
 */
static int write_compressed_function_call(ref_node_t* node, fmt_data_t* fmt)
{
	rd_fcall_t* call = (rd_fcall_t*)node->ref;

	/* first write all comments with index less than current call index */
	if (fmt->comment) {
		fmt->comment = dlist_foreach2_in(&fmt->rd->comments, fmt->comment,
				(op_binary_t)comment_check_index, (void*)(long)call->index, (op_binary_t)formatter_write_comment, fmt->fp);
	}

	/* then write the function call itself */
	formatter_write_fcall(call, fmt->fp);
	if (call->args) formatter_write_fargs(call->args, fmt->fp);
	return 0;
}


/**
 * Writes compressed backtrace data into text log.
 *
 * The compressed backtrace data consists of all associated function data
 * plus the backtrace itself. Basically it means grouping function calls
 * by backtraces.
 * @param[in] trace   the backtrace data.
 * @param[in] fmt     the log file handle.
 * @return
 */
static int write_compressed_backtrace(ftrace_ref_t* trace, fmt_data_t* fmt)
{
	dlist_foreach2(&trace->ref->calls, (op_binary_t)write_compressed_function_call, fmt);
	formatter_write_ftrace(trace->ref, fmt->fp);
	return 0;
}

/**
 * Writes heap statistics information.
 *
 * @param[in] fp     the output stream.
 * @param[in] hinfo  the heap information data.
 * @return
 */
static void write_heap_information(FILE* fp, rd_hinfo_t* hinfo)
{
	formatter_printf(fp, "# heap status information:\n");
	formatter_printf(fp, "#   heap bottom %p\n", hinfo->heap_bottom);
	formatter_printf(fp, "#   heap top %p\n", hinfo->heap_top);
	formatter_printf(fp, "#   lowest block %p\n", hinfo->lowest_block);
	formatter_printf(fp, "#   highest block %p\n", hinfo->highest_block);
	formatter_printf(fp, "#   non-mapped space allocated from system %d\n", hinfo->arena);
	formatter_printf(fp, "#   number of free chunks %d\n", hinfo->ordblks);
	formatter_printf(fp, "#   number of fastbin blocks %d\n", hinfo->smblks);
	formatter_printf(fp, "#   number of mapped regions %d\n", hinfo->hblks);
	formatter_printf(fp, "#   space in mapped regions %d\n", hinfo->hblkhd);
	formatter_printf(fp, "#   maximum total allocated space %d\n", hinfo->usmblks);
	formatter_printf(fp, "#   space available in freed fastbin blocks %d\n", hinfo->fsmblks);
	formatter_printf(fp, "#   total allocated space %d\n", hinfo->uordblks);
	formatter_printf(fp, "#   total free space %d\n", hinfo->fordblks);
	formatter_printf(fp, "#   top-most, releasable (via malloc_trim) space %d\n", hinfo->keepcost);
}

typedef struct {
	FILE* fp;
	leak_data_t leaks[32];
} leaks_t;

static void write_leaks(rd_resource_t* res, leaks_t* leaks)
{
	formatter_printf(leaks->fp, "# Resource - %s:\n", res->name);
	int index = ffs(res->id) - 1;
	formatter_printf(leaks->fp, "# %d block(s) leaked with total size of %d bytes\n", leaks->leaks[index].count, leaks->leaks[index].total_size);

}

/*
 * Public API
 */

void write_leak_summary(fmt_data_t* fmt)
{
	leaks_t leaks = {.fp = fmt->fp};
	dlist_foreach2(&fmt->rd->calls, (op_binary_t)sum_leaks, leaks.leaks);

	dlist_foreach2(&fmt->rd->resources, (op_binary_t)write_leaks, &leaks);
}


void write_trace_environment(fmt_data_t* fmt)
{
	/* prepare version and timestamp strings */
	char version[8], timestamp[64], spid[8];
	sprintf(spid, "%d", fmt->rd->pinfo->pid);
	sprintf(version, "%d.%d", fmt->rd->hshake->vmajor, fmt->rd->hshake->vminor);
	struct tm* tm = localtime(&fmt->rd->pinfo->timestamp.tv_sec);
	sprintf(timestamp, "%d.%d.%d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

	/* initialize header data structure */
	formatter_header_t header[] = {
			{.key = SP_RTRACE_FORMATTER_HEADER_VERSION, .value = version},
			{.key = SP_RTRACE_FORMATTER_HEADER_ARCH, .value = fmt->rd->hshake->arch},
			{.key = SP_RTRACE_FORMATTER_HEADER_TIMESTAMP, .value = timestamp},
			{.key = SP_RTRACE_FORMATTER_HEADER_PROCESS, .value = fmt->rd->pinfo->name},
			{.key = SP_RTRACE_FORMATTER_HEADER_PID, .value = spid},
			{.key = NULL, .value = NULL}
	};
	/* write header data */
	formatter_write_header(header, fmt->fp);

	/* write heap information if exists */
	if (fmt->rd->hinfo) write_heap_information(fmt->fp, fmt->rd->hinfo);

	/* write tracing module data */
	dlist_foreach2(&fmt->rd->minfo, (op_binary_t)write_module_info, fmt->fp);

	/* write context registry */
	dlist_foreach2(&fmt->rd->contexts, (op_binary_t)formatter_write_context, fmt->fp);

	/* write resource registry */
	dlist_foreach2(&fmt->rd->resources, (op_binary_t)formatter_write_resource, fmt->fp);

	/* write memory mapping data */
	dlist_foreach2(&fmt->rd->mmaps, (op_binary_t)formatter_write_mmap, fmt->fp);
}

void write_trace_calls(fmt_data_t* fmt)
{
	/* write the function call data (with backtraces and arguments) */
	if (postproc_options.compress) {
		dlist_t leaks;
		dlist_init(&leaks);
		leaks_sort(&fmt->rd->ftraces, &leaks, postproc_options.compare_leaks);
		dlist_foreach2(&leaks, (op_binary_t)write_compressed_backtrace, fmt);
		dlist_free(&leaks, (op_unary_t)free);
	}
	else {
		dlist_foreach2(&fmt->rd->calls, (op_binary_t)write_function_call, fmt);
	}
}
