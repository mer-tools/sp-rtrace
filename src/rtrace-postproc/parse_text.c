/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010,2011 by Nokia Corporation
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
#include "config.h"

#include <stdio.h>
#include <time.h>
#include <limits.h>

#include "common/sp_rtrace_proto.h"
#include "library/sp_rtrace_defs.h"
#include "common/header.h"
#include "sp_rtrace_postproc.h"
#include "library/sp_rtrace_formatter.h"
#include "library/sp_rtrace_parser.h"

#include "parse_text.h"
#include "common/utils.h"

/**
 * Parses text format header.
 *
 * @param[in] rd     the resource trace data.
 * @param[in] line   the trace log header.
 */
static void parse_header(rd_t* rd, const char* line)
{
	/* create handshake and process info packets which are construed
	 * from header data.
	 * TODO: Add endianness & pointer size data to header ?
	 */
	rd->hshake = calloc_a(1, sizeof(rd_hshake_t));
	rd->pinfo = calloc_a(1, sizeof(rd_pinfo_t));

	sp_rtrace_header_t header;
	sp_rtrace_parser_parse_header(line, &header);
	
	if (header.fields[SP_RTRACE_HEADER_VERSION]) {
		int vmajor, vminor;
		if (sscanf(header.fields[SP_RTRACE_HEADER_VERSION], "%d.%d", &vmajor, &vminor) == 2) {
			rd->hshake->vmajor = vmajor;
			rd->hshake->vminor = vminor;
		}
	}
	if (header.fields[SP_RTRACE_HEADER_ARCH]) {
		rd->hshake->arch = strdup_a(header.fields[SP_RTRACE_HEADER_ARCH]);
	}

	if (header.fields[SP_RTRACE_HEADER_TIMESTAMP]) {
		struct tm tm = {.tm_isdst = daylight};
		if (sscanf(header.fields[SP_RTRACE_HEADER_TIMESTAMP], "%d.%d.%d %d:%d:%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
				&tm.tm_hour, &tm.tm_min, &tm.tm_sec) == 6) {
			tm.tm_year -= 1900;
			tm.tm_mon--;
			rd->pinfo->timestamp.tv_sec = mktime(&tm);
			rd->pinfo->timestamp.tv_usec = 0;
		}
	}

	if (header.fields[SP_RTRACE_HEADER_PROCESS]) {
		rd->pinfo->name = strdup_a(header.fields[SP_RTRACE_HEADER_PROCESS]);
	}
	if (header.fields[SP_RTRACE_HEADER_ORIGIN]) {
		rd->pinfo->trace_origin = strdup_a(header.fields[SP_RTRACE_HEADER_ORIGIN]);
	}
	if (header.fields[SP_RTRACE_HEADER_PID]) {
		rd->pinfo->pid = atoi(header.fields[SP_RTRACE_HEADER_PID]);
	}
	if (header.fields[SP_RTRACE_HEADER_FILTER]) {
		rd->filter = header_get_filter(&header);
	}
	if (header.fields[SP_RTRACE_HEADER_BACKTRACE_DEPTH]) {
		rd->pinfo->backtrace_depth = atoi(header.fields[SP_RTRACE_HEADER_BACKTRACE_DEPTH]);
	}
	else {
		rd->pinfo->backtrace_depth = -1;
	}
	sp_rtrace_parser_free_header(&header);
}


/**
 * Creates and stores new backtrace data object.
 *
 * @param[in] rd     the resource trace data.
 * @param[in] call   the source function call object.
 * @param[in] bt     the backtrace data.
 * @param[in] size   the backtrace size (number of addresses).
 * @return
 */
static void store_backtrace(rd_t* rd, dlist_t* calls, sp_rtrace_btframe_t* bt, int size)
{
	int i;

	rd_ftrace_t* trace = htable_create_node(sizeof(rd_ftrace_t));
	trace->data.resolved_names = NULL;
	trace->ref_count = 0;
	trace->data.nframes = size;
	trace->data.frames = size ? malloc_a(size * sizeof(rd_ftrace_t)) : NULL;
	dlist_init(&trace->calls);
	/* copy the collected backtrace data */
	for (i = 0; i < size; i++) {
		trace->data.frames[i] = bt[i].addr;
		if (bt[i].name) {
			if (trace->data.resolved_names == NULL) {
				trace->data.resolved_names = calloc_a(size, sizeof(char*));
			}
			trace->data.resolved_names[i] = bt[i].name;
			bt[i].name = NULL;
		}
	}
	rd_fcalls_set_ftrace(rd, calls, trace);
	dlist_free(calls, NULL);
}

/**
 * Creates and stores new function argument data object.
 *
 * @param[in] rd     the resource trace data.
 * @param[in] call   the source function call object.
 * @param[in] args   the function arguments.
 * @param[in] size   the number of function arguments.
 * @return
 */
static void store_call_arguments(rd_fcall_t* call, sp_rtrace_farg_t* args, int size)
{
	rd_fargs_t* fargs = (rd_fargs_t*)malloc_a(sizeof(rd_fargs_t));
	fargs->data = malloc_a(sizeof(sp_rtrace_farg_t) * (size + 1));
	memcpy(fargs->data, args, sizeof(sp_rtrace_farg_t) * size);
	fargs->data[size].name = NULL;
	fargs->data[size].value = NULL;
	call->args = fargs;
}

/**
 * Compare operation for resource lookup.
 */
static long compare_resource(rd_resource_t* res, sp_rtrace_resource_t* template)
{
	return strcmp(res->data.type, template->type);
}

/**
 * Compare calls by timestamp/index
 * @param call1
 * @param call2
 * @return
 */
static long compare_calls(rd_fcall_t* call1, rd_fcall_t* call2)
{
	if (call1->data.timestamp == call2->data.timestamp) return call1->data.index - call2->data.index;
	return call1->data.timestamp - call2->data.timestamp;
}

/**
 * Reads and parses text format trace log.
 *
 * @param[in] rd   the resource trace data container.
 * @param[in] fp   the input stream file pointer.
 * @return
 */
static void read_text_data(rd_t* rd, FILE* fp)
{
	char line[4096];
	/* the last (previous) function call */
	dlist_t last_calls;
	/* temporary backtrace step storage */
	int bt_limit = 100;
	int bt_index = 0;
	/* temporary function argument storage */
	int args_limit = 10;
	int args_index = 0;

	/* read and parse header line */
	char* ptr = fgets(line, sizeof(line), fp);
	if (ptr == NULL) {
		return;
	}
	parse_header(rd, line);

	dlist_init(&last_calls);

	sp_rtrace_btframe_t* bt = malloc_a(sizeof(sp_rtrace_btframe_t) * bt_limit);
	sp_rtrace_farg_t* args = malloc_a(sizeof(sp_rtrace_farg_t) * args_limit);

	/* The associated function call index for comments. Practically
	 * its the last call index. Used for comment ordering in output
	 * file. Comments are always printed before associated function
	 * call index + 1 */
	int comment_index = 0;

	/* read and parse the rest of file */
	while (fgets(line, sizeof(line), fp) && !postproc_abort) {
		/* discard any temporary (starting with '# ') comments */
		if (line[0] == '#' && line[1] == ' ') continue;

		sp_rtrace_record_t rec;

		int rec_type = sp_rtrace_parser_parse_record(line, &rec);

		if (rec_type == SP_RTRACE_RECORD_TRACE) {
			if (dlist_first(&last_calls)) {
				bt[bt_index++] = rec.frame;
				if (bt_index == bt_limit) {
					bt_limit *= 2;
					bt = realloc_a(bt, sizeof(sp_rtrace_btframe_t) * bt_limit);
				}
			}
			else {
				fprintf(stderr, "WARNING: found a backtrace record without preceding function call record\n");
			}
			continue;
		}

		if (rec_type == SP_RTRACE_RECORD_ARG) {
			if (dlist_first(&last_calls)) {
				args[args_index++] = rec.arg;
				args_index++;
				if (args_index == args_limit) {
					args_limit *= 2;
					args = realloc_a(args, sizeof(sp_rtrace_farg_t) * args_limit);
				}
			}
			else {
				fprintf(stderr, "WARNING: found a function argument record without preceding function call record\n");
			}
			continue;
		}

		/* if args_index is set at this place, it means that parser finished
		 * to process all function argument records belonging to the last call
		 * and thus a function argument data object created and stored.
		 */
		if (args_index) {
			store_call_arguments(REF_NODE(dlist_last(&last_calls))->ref, args, args_index);
			args_index = 0;
		}
		/* if bt_index is set at this place, it means that parser finished
		 * to process all trace records belonging to single backtrace and
		 * thus a backtrace data object can be created and stored.
		 */
		if ( (bt_index || *line == '\n') && dlist_first(&last_calls) ) {
			store_backtrace(rd, &last_calls, bt, bt_index);
			bt_index = 0;
		}
		
		/* empty line indicates end of a backtrace */
		if (*line == '\n') {
			continue;
		}

		if (rec_type == SP_RTRACE_RECORD_CALL) {
			rd_fcall_t* call = dlist_create_node(sizeof(rd_fcall_t));
			call->data = rec.call;
			call->trace = NULL;
			call->args = NULL;
			call->ref = NULL;

			/* The res_type field temporary has the resource type name string assigned.
			 * Lookup the resource record and correctly assign to it.
			 * The resource type name is stored in a static buffer, so it must be left unfreed */
			if (call->data.res_type_flag == SP_RTRACE_FCALL_RFIELD_NAME) {
				rd_resource_t resource = {.data = {.type = (char*)call->data.res_type} };
				call->data.res_type = dlist_find(&rd->resources, (void*)&resource, (op_binary_t)compare_resource);
			}
			else {
				/* If resource type was not set in text log, it means only one resource type is present.
				 * So assign the first (and only) resource to call record. */
				call->data.res_type = (rd_resource_t*)dlist_first(&rd->resources);
			}
			call->data.res_type_flag = SP_RTRACE_FCALL_RFIELD_REF;
			dlist_add(&rd->calls, call);

			ref_node_t* ref = (ref_node_t*)dlist_create_node(sizeof(ref_node_t));
			ref->ref = call;
			dlist_add(&last_calls, ref);
			comment_index = call->data.index;
			continue;
		}

		/* no backtrace records found for the last call - assign empty backtrace record */
		if (dlist_first(&last_calls)) {
			if ( ! RD_FCALL(REF_NODE(dlist_first(&last_calls))->ref)->trace) store_backtrace(rd, &last_calls, NULL, 0);

			/* reset last call variable if the last packet was not
			 * function call record or trace */
		}

		/* check if the line contains memory mapping record */
		if (rec_type == SP_RTRACE_RECORD_MMAP) {
			rd_mmap_t* mmap = dlist_create_node(sizeof(rd_mmap_t));
			mmap->data = rec.mmap;
			dlist_add(&rd->mmaps, mmap);
			continue;
		}

		if (rec_type == SP_RTRACE_RECORD_CONTEXT) {
			rd_context_t* context = dlist_create_node(sizeof(rd_context_t));
			dlist_add(&rd->contexts, context);
			continue;
		}

		if (rec_type == SP_RTRACE_RECORD_RESOURCE) {
			if (!dlist_find(&rd->resources, &rec.resource, (op_binary_t)compare_resource)) {
				rd_resource_t* resource = dlist_create_node(sizeof(rd_resource_t));
				resource->data = rec.resource;
				resource->hide = false;
				dlist_add(&rd->resources, resource);
			}
			else {
				sp_rtrace_parser_free_record(rec_type, &rec);
			}
			continue;
		}

		if (rec_type == SP_RTRACE_RECORD_ATTACHMENT) {
			rd_attachment_t* file = dlist_create_node(sizeof(rd_attachment_t));
			file->data = rec.attachment;
			dlist_add(&rd->files, file);
			continue;
		}

		if (rec_type == SP_RTRACE_RECORD_COMMENT) {
			if (*line != '#') {
				fprintf(stderr, "WARNING, unrecognized record, assuming it's a comment: %s", line);
			}
			rd_comment_t* comment = dlist_create_node(sizeof(rd_comment_t));
			comment->index = comment_index;
			/* strip the trailing LF character */
			line[strlen(line)] = '\0';
			comment->text = strdup_a(line);
			dlist_add(&rd->comments, comment);
		}
	}
	if (args_index) {
		store_call_arguments(REF_NODE(dlist_last(&last_calls))->ref, args, args_index);
	}
	if (dlist_first(&last_calls)) {
		store_backtrace(rd, &last_calls, bt, bt_index);
	}

	free(bt);
	free(args);

	dlist_sort(&rd->calls, (op_binary_t)compare_calls);
}


/*
 * Public API implementation.
 */
void process_text_data(rd_t* rd, FILE* fp)
{
	read_text_data(rd, fp);

	if (postproc_options.input_file) {
		fclose(fp);
	}
}
