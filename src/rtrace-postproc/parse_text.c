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
#include "config.h"

#include <stdio.h>
#include <time.h>
#include <limits.h>

#include "common/sp_rtrace_proto.h"
#include "common/header.h"
#include "sp_rtrace_postproc.h"

#include "parse_text.h"
#include "common/formatter.h"
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

	header_t header;
	header_read(&header, line);
	
	if (header.fields[HEADER_VERSION]) {
		int vmajor, vminor;
		if (sscanf(header.fields[HEADER_VERSION], "%d.%d", &vmajor, &vminor) == 2) {
			rd->hshake->vmajor = vmajor;
			rd->hshake->vminor = vminor;
		}
	}
	if (header.fields[HEADER_ARCH]) {
		rd->hshake->arch = strdup_a(header.fields[HEADER_ARCH]);
	}

	if (header.fields[HEADER_TIMESTAMP]) {
		struct tm tm = {.tm_isdst = daylight};
		if (sscanf(header.fields[HEADER_TIMESTAMP], "%d.%d.%d %d:%d:%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
				&tm.tm_hour, &tm.tm_min, &tm.tm_sec) == 6) {
			tm.tm_year -= 1900;
			tm.tm_mon--;
			rd->pinfo->timestamp.tv_sec = mktime(&tm);
			rd->pinfo->timestamp.tv_usec = 0;
		}
	}

	if (header.fields[HEADER_PROCESS]) {
		rd->pinfo->name = strdup_a(header.fields[HEADER_PROCESS]);
	}
	if (header.fields[HEADER_PID]) {
		rd->pinfo->pid = atoi(header.fields[HEADER_PID]);
	}
	if (header.fields[HEADER_FILTER]) {
		rd->filter = header_get_filter(&header);
	}
	if (header.fields[HEADER_BACKTRACE_DEPTH]) {
		rd->pinfo->backtrace_depth = atoi(header.fields[HEADER_BACKTRACE_DEPTH]);
	}
	else {
		rd->pinfo->backtrace_depth = -1;
	}
	header_free(&header);
}

/**
 * Parses memory mapping record.
 *
 * @param[in] line   the line to parse.
 * @return           the memory mapping record,
 *                   or NULL if parsing failed (not a memory mapping record).
 */
static void* parse_memory_mapping(char* line)
{
	rd_mmap_t* mmap = NULL;
	char module[PATH_MAX];
	pointer_t from, to;
	if (sscanf(line, ": %s => 0x%lx-0x%lx", module, &from, &to) == 3) {
		mmap = 	(rd_mmap_t*)dlist_create_node(sizeof(rd_mmap_t));
		mmap->module = strdup_a(module);
		mmap->from = from;
		mmap->to = to;
	}
	return mmap;
}

/**
 * Parses context registry record.
 *
 * @param[in] line   the line to parse.
 * @return           the context registry record,
 *                   or NULL if parsing failed (not a context registry record).
 */
static void* parse_context_registry(char* line)
{
	rd_context_t* context = NULL;
	char name[512];
	unsigned int id;
	if (sscanf(line, "@ %x : %[^\n]", &id, name) == 2) {
		context = dlist_create_node(sizeof(rd_context_t));
		context->id = id;
		context->name = strdup_a(name);
	}
	return context;
}

/**
 * Parses resource registry record.
 *
 * @param[in] line   the line to parse.
 * @return           the resource registry record,
 *                   or NULL if parsing failed (not a resource registry record).
 */
static void* parse_resource_registry(char* line)
{
	rd_resource_t* resource = NULL;
	char type[512], desc[512];
	unsigned int id;
	if (sscanf(line, "<%x> : %[^ ] (%[^)])", &id, type, desc) == 3) {
		resource = dlist_create_node(sizeof(rd_resource_t));
		resource->id = ffs(id);
		resource->type = strdup_a(type);
		resource->desc = strdup_a(desc);
		resource->hide = false;
	}
	return resource;
}


/**
 * Parses function call record.
 *
 * @param[in] line   the line to parse.
 * @return           the function call record,
 *                   or NULL if parsing failed (not a function call record).
 */
static void* parse_function_call(char* line)
{
	static char res_type_name[512];
	int index, context = 0;
	int hours, minutes, seconds, mseconds;
	int timestamp = 0;
	pointer_t res_id;
	int res_size;
	char name[512], *ptr = line, delim, function_type;
	char* res_type;
	/* parse index field <index>. */
	if (sscanf(ptr, "%d%c", &index, &delim) != 2 || delim != '.') return NULL;
	/* move cursor beyond index field */
	ptr = strchr(ptr, ' ');
	if (!ptr) return NULL;
	ptr++;

	/* parse optional context mask */
	if (sscanf(ptr, "@%x", &context) == 1) {
		/* context mask was parsed successfully. Move cursor to next field */
		ptr = strchr(ptr, '@');
		ptr = strchr(ptr, ' ');
		if (!ptr) return NULL;
		ptr++;
	}
	/* parse optional timestamp */
	if (sscanf(ptr, "[%d:%d:%d.%d]", &hours, &minutes, &seconds, &mseconds) == 4) {
		/* timestamp was parsed successfully. Move cursor beyond timestamp */
		timestamp = hours * 60 * 60 * 1000 + minutes * 60 * 1000 + seconds * 1000 + mseconds;
		ptr = strchr(ptr, ' ');
		if (!ptr) return NULL;
		ptr++;
	}
	if (sscanf(ptr, "%[^(<]", name) != 1) {
		return NULL;
	}
	ptr += strlen(name);
	if (*ptr == '<') {
		if (sscanf(ptr, "<%[^>]>", res_type_name) == 0) {
			return NULL;
		}
		ptr = strchr(ptr, '(');
		if (!ptr) return NULL;
		res_type = res_type_name;
	}
	else {
		res_type = NULL;
	}
	if (sscanf(ptr, "(%d) = 0x%lx", &res_size, &res_id) == 2) {
		function_type = SP_RTRACE_FTYPE_ALLOC;
	}
	else if (sscanf(ptr, "(0x%lx)", &res_id) == 1) {
		res_size = 0;
		function_type = SP_RTRACE_FTYPE_FREE;
	}
	else {
		/* unknown format, return NULL */
		return NULL;
	}
	rd_fcall_t* call = dlist_create_node(sizeof(rd_fcall_t));
	call->index = index;
	/* temporary assign the resource type name to res_type field. After returning
	 * from this function the correct resource type structure will be found and
	 * assigned instead. */
	call->res_type = (rd_resource_t*)(long)res_type;

	call->type = function_type;
	call->context = context;
	call->name = strdup_a(name);
	call->res_id = res_id;
	call->res_size = (long)res_size;
	call->timestamp = timestamp;
	call->trace = NULL;
	call->args = NULL;
	call->ref = NULL;
	return call;
}

/**
 * Single backtrace step data,
 */
typedef struct bt_step_t {
	pointer_t addr;
	char* name;
} bt_step_t;


/**
 * Parses backtrace record.
 *
 * @param[in] line     the line to parse.
 * @param[out] btstep  the output data structure.
 * @return             true if backtrace was parsed successfully,
 *                     false otherwise (not a backtrace record).
 */
static bool parse_backtrace(char* line, bt_step_t* btstep)
{
	char resolved_name[PATH_MAX], delim;
	int n = sscanf(line, "%c0x%lx %[^\n]", &delim, &btstep->addr, resolved_name);
	if (n < 2 || delim != '\t') return false;
	if (n == 3) {
		btstep->name = strdup_a(resolved_name);
	}
	else {
		btstep->name = NULL;
	}
	return true;
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
static void store_backtrace(rd_t* rd, dlist_t* calls, bt_step_t* bt, int size)
{
	int i;

	rd_ftrace_t* trace = htable_create_node(sizeof(rd_ftrace_t));
	trace->resolved_names = NULL;
	trace->ref_count = 0;
	trace->nframes = size;
	trace->frames = size ? malloc_a(size * sizeof(rd_ftrace_t)) : NULL;
	dlist_init(&trace->calls);
	/* copy the collected backtrace data */
	for (i = 0; i < size; i++) {
		trace->frames[i] = bt[i].addr;
		if (bt[i].name) {
			if (trace->resolved_names == NULL) {
				trace->resolved_names = calloc_a(size, sizeof(char*));
			}
			trace->resolved_names[i] = bt[i].name;
			bt[i].name = NULL;
		}
	}
	rd_fcalls_set_ftrace(rd, calls, trace);
	dlist_free(calls, NULL);
}

/**
 * Parses function argument record.
 *
 * @param[in] line    the line to parse.
 * @param[out] parg   the output pointer.
 * @return            true if function argument was parsed successfully,
 *                    false otherwise (not a function argument record).
 */
static bool parse_arguments(char* line, char** parg)
{
	char arg[PATH_MAX], delim;
	int index;

	int n = sscanf(line, "%c$%d = %s", &delim, &index, arg);
	if (n < 3 || delim != '\t') return false;
	*parg = strdup_a(arg);
	return true;
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
static void store_call_arguments(rd_fcall_t* call, char** args, int size)
{
	rd_fargs_t* fargs = (rd_fargs_t*)malloc_a(sizeof(rd_fargs_t));
	fargs->args = malloc_a(sizeof(char*) * (size + 1));
	memcpy(fargs->args, args, sizeof(char*) * size);
	fargs->args[size] = NULL;
	call->args = fargs;
}

/**
 * Compare operation for resource lookup.
 */
static long compare_resource(rd_resource_t* res, rd_resource_t* template)
{
	return strcmp(res->type, template->type);
}

/**
 * Compare calls by timestamp/index
 * @param call1
 * @param call2
 * @return
 */
static long compare_calls(rd_fcall_t* call1, rd_fcall_t* call2)
{
	if (call1->timestamp == call2->timestamp) return call1->index - call2->index;
	return call1->timestamp - call2->timestamp;
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
	void* data;
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

	bt_step_t* bt = malloc_a(sizeof(bt_step_t) * bt_limit);
	char** args = malloc_a(sizeof(char*) * args_limit);

	/* The associated function call index for comments. Practically
	 * its the last call index. Used for comment ordering in output
	 * file. Comments are always printed before associated function
	 * call index + 1 */
	int comment_index = 0;

	/* read and parse the rest of file */
	while (fgets(line, sizeof(line), fp) && !postproc_abort) {
		/* discard any temporary (starting with '# ') comments */
		if (line[0] == '#' && line[1] == ' ') continue;
		/* check if the line contains backtrace record */
		if (dlist_first(&last_calls) && parse_backtrace(line, &bt[bt_index])) {
			bt_index++;
			if (bt_index == bt_limit) {
				bt_limit *= 2;
				bt = realloc_a(bt, sizeof(bt_step_t) * bt_limit);
			}
			continue;
		}
		/* check if the line contains function argument record */
		if (dlist_first(&last_calls) && parse_arguments(line, &args[args_index])) {
			args_index++;
			if (args_index == args_limit) {
				args_limit *= 2;
				args = realloc_a(args, sizeof(char*) * args_limit);
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

		/* check if the line contains function call record */
		data = parse_function_call(line);
		if (data) {
			/* The res_type field temporary has the resource type name string assigned.
			 * Lookup the resource record and correctly assign to it.
			 * The resource type name is stored in a static buffer, so it must be left unfreed */
			if (RD_FCALL(data)->res_type) {
				rd_resource_t resource = {.type = (char*)RD_FCALL(data)->res_type};
				RD_FCALL(data)->res_type = dlist_find(&rd->resources, (void*)&resource,	(op_binary_t)compare_resource);
			}
			else {
				/* If resource type was not set in text log, it means only one resource type is present.
				 * So assign the first (and only) resource to call record. */
				RD_FCALL(data)->res_type = (rd_resource_t*)dlist_first(&rd->resources);
			}

			dlist_add(&rd->calls, data);
			ref_node_t* ref = (ref_node_t*)dlist_create_node(sizeof(ref_node_t));
			ref->ref = data;
			dlist_add(&last_calls, ref);
			comment_index = RD_FCALL(data)->index;
			continue;
		}

		/* no backtrace records found for the last call - assign empty backtrace record */
		if (dlist_first(&last_calls)) {
			if ( ! RD_FCALL(REF_NODE(dlist_first(&last_calls))->ref)->trace) store_backtrace(rd, &last_calls, NULL, 0);

			/* reset last call variable if the last packet was not
			 * function call record or trace */
		}

		/* check if the line contains memory mapping record */
		data = parse_memory_mapping(line);
		if (data) {
			dlist_add(&rd->mmaps, data);
			continue;
		}
		/* check if the line contains context registry record */
		data = parse_context_registry(line);
		if (data) {
			dlist_add(&rd->contexts, data);
			continue;
		}
		/* check if the line contains resource registry record */
		data = parse_resource_registry(line);
		if (data) {
			if (!dlist_find(&rd->resources, data, (op_binary_t)compare_resource)) {
				dlist_add(&rd->resources, data);
			}
			else {
				rd_resource_free((rd_resource_t*)data);
			}
			continue;
		}
		/* otherwise assume that line contains comment record */
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
