/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2011-2012 by Nokia Corporation
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
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "sp_rtrace_defs.h"

#include "sp_rtrace_parser.h"

#include "common/utils.h"
#include "library/sp_rtrace_formatter.h"

static int parse_record_mask = SP_RTRACE_RECORD_ALL;

enum {
	PARSE_FAIL = -1,
	PARSE_OK = 0,
	PARSE_IGNORE = 1,
};


/**
 * Parses trace frame record from the input text.
 *
 * @param[in] line   the text to parse.
 * @param[out] data  the parsed data.
 * @return           PARSE_FAIL   - the input text does not contain trace frame data.
 *                   PARSE_OK     - the trace frame data was parsed successfully.
 *                   PARSE_IGNORE - the input text contains trace frame data, but was
 *                                  set to be ignored by sp_rtrace_parser_set_mask()
 *                                  function.
 */
static int parse_backtrace(const char* line, sp_rtrace_btframe_t* data)
{
	if (*line != '\t') return PARSE_FAIL;

	char resolved_name[PATH_MAX];
	int n = sscanf(line, "\t0x%lx %[^\n]", &data->addr, resolved_name);
	if (n < 1) return PARSE_FAIL;
	if ( !(parse_record_mask & SP_RTRACE_RECORD_TRACE) ) return PARSE_IGNORE;
	data->name = (n == 2) ? strdup_a(resolved_name) : NULL;
	return PARSE_OK;
}

/**
 * Parses function calll record from the input text.
 *
 * @param[in] line   the text to parse.
 * @param[out] data  the parsed data.
 * @return           PARSE_FAIL   - the input text does not contain function call data.
 *                   PARSE_OK     - the function call data was parsed successfully.
 *                   PARSE_IGNORE - the input text contains trace function call, but was
 *                                  set to be ignored by sp_rtrace_parser_set_mask()
 *                                  function.
 */
static int parse_function_call(const char* line, sp_rtrace_fcall_t* data)
{
	static char res_type_name[512];
	int idx, context = 0;
	int hours, minutes, seconds, mseconds;
	int timestamp = 0;
	pointer_t res_id;
	int res_size;
	char name[512], delim, function_type;
	const char* ptr = line;
	unsigned int res_type_flag = SP_RTRACE_FCALL_RFIELD_UNDEF;

	/* parse index field <index>. */
	if (sscanf(ptr, "%d%c", &idx, &delim) != 2 || delim != '.') return PARSE_FAIL;
	/* move cursor beyond index field */
	ptr = strchr(ptr, ' ');
	if (!ptr) return PARSE_FAIL;
	ptr++;

	/* parse optional context mask */
	if (sscanf(ptr, "@%x", &context) == 1) {
		/* context mask was parsed successfully. Move cursor to next field */
		ptr = strchr(ptr, '@');
		ptr = strchr(ptr, ' ');
		if (!ptr) return PARSE_FAIL;
		ptr++;
	}
	/* parse optional timestamp */
	if (sscanf(ptr, "[%d:%d:%d.%d]", &hours, &minutes, &seconds, &mseconds) == 4) {
		/* timestamp was parsed successfully. Move cursor beyond timestamp */
		timestamp = hours * 60 * 60 * 1000 + minutes * 60 * 1000 + seconds * 1000 + mseconds;
		ptr = strchr(ptr, ' ');
		if (!ptr) return PARSE_FAIL;
		ptr++;
	}
	char* ptr2 = strrchr(ptr, '(');
	if (!ptr2) return PARSE_FAIL;
	if (*(ptr2 - 1) == '>') ptr2 = strrchr(ptr, '<');

	memcpy(name, ptr, ptr2 - ptr);
	name[ptr2 - ptr] = '\0';
	ptr = ptr2;
	if (*ptr == '<') {
		if (sscanf(ptr, "<%[^>]>", res_type_name) == 0) {
			return PARSE_FAIL;
		}
		ptr = strchr(ptr, '(');
		if (!ptr) return PARSE_FAIL;
		res_type_flag = SP_RTRACE_FCALL_RFIELD_NAME;
	}
	if (sscanf(ptr, "(%d) = 0x%lx", &res_size, &res_id) == 2) {
		function_type = SP_RTRACE_FTYPE_ALLOC;
	}
	else if (sscanf(ptr, "(0x%lx)", &res_id) == 1) {
		res_size = 0;
		function_type = SP_RTRACE_FTYPE_FREE;
	}
	else {
		return PARSE_FAIL;
	}
	if ( !(parse_record_mask & SP_RTRACE_RECORD_CALL) ) return PARSE_IGNORE;

	data->index = idx;
	/* temporary assign the resource type name to res_type field. After returning
	 * from this function the correct resource type structure will be found and
	 * assigned instead. */
	data->res_type_flag = res_type_flag;
	data->res_type = res_type_flag == SP_RTRACE_FCALL_RFIELD_NAME ? strdup_a(res_type_name) : NULL;
	data->type = function_type;
	data->context = context;
	data->name = strdup_a(name);
	data->res_id = res_id;
	data->res_size = (long)res_size;
	data->timestamp = timestamp;

	return PARSE_OK;
}

/**
 * Parses function argument record from the input text.
 *
 * @param[in] line   the text to parse.
 * @param[out] data  the parsed data.
 * @return           PARSE_FAIL   - the input text does not contain function argument data.
 *                   PARSE_OK     - the function argument data was parsed successfully.
 *                   PARSE_IGNORE - the input text contains trace function argument, but was
 *                                  set to be ignored by sp_rtrace_parser_set_mask()
 *                                  function.
 */
static int parse_arguments(const char* line, sp_rtrace_farg_t* data)
{
	char value[PATH_MAX], name[128];
	if (*line != '\t' || sscanf(line, "\t$%s = %s", name, value) != 2) return PARSE_FAIL;
	if ( !(parse_record_mask & SP_RTRACE_RECORD_ARG) ) return PARSE_IGNORE;

	data->name = strdup_a(name);
	data->value = strdup_a(value);
	return PARSE_OK;
}

/**
 * Parses memory mapping record from the input text.
 *
 * @param[in] line   the text to parse.
 * @param[out] data  the parsed data.
 * @return           PARSE_FAIL   - the input text does not contain memory mapping data.
 *                   PARSE_OK     - the memory mapping data was parsed successfully.
 *                   PARSE_IGNORE - the input text contains trace memory mapping, but was
 *                                  set to be ignored by sp_rtrace_parser_set_mask()
 *                                  function.
 */
static int parse_memory_mapping(const char* line, sp_rtrace_mmap_t* data)
{
	char module[PATH_MAX];
	pointer_t from, to;
	if (sscanf(line, ": %s => 0x%lx-0x%lx", module, &from, &to) != 3) return PARSE_FAIL;
	if ( !(parse_record_mask & SP_RTRACE_RECORD_MMAP) ) return PARSE_IGNORE;
	data->module = strdup_a(module);
	data->from = from;
	data->to = to;
	return PARSE_OK;
}


/**
 * Parses context registry record from the input text.
 *
 * @param[in] line   the text to parse.
 * @param[out] data  the parsed data.
 * @return           PARSE_FAIL   - the input text does not contain context registry data.
 *                   PARSE_OK     - the context registry data was parsed successfully.
 *                   PARSE_IGNORE - the input text contains trace context registry, but was
 *                                  set to be ignored by sp_rtrace_parser_set_mask()
 *                                  function.
 */
static int parse_context_registry(const char* line, sp_rtrace_context_t* data)
{
	char name[512];
	unsigned int id;
	if (sscanf(line, "@ %x : %[^\n]", &id, name) != 2) return PARSE_FAIL;
	if ( !(parse_record_mask & SP_RTRACE_RECORD_CONTEXT) ) return PARSE_IGNORE;
	data->id = id;
	data->name = strdup_a(name);
	return PARSE_OK;
}

/**
 * Parses resource type flags from input text.
 *
 * @param[in] text  the text containing resource type flags in textual format.
 * @return          the resource type flags.
 */
static unsigned int parse_resource_flags(const char* text) {
	unsigned int nflag = 0, flag, flags = 0;

	while ( (flag = 1 << nflag) <= SP_RTRACE_RESOURCE_LAST_FLAG ) {
		if (strstr(text, sp_rtrace_resource_flags_text[nflag++])) flags |= flag;
	}
	return flags;
}

/**
 * Parses resource registry record from the input text.
 *
 * @param[in] line   the text to parse.
 * @param[out] data  the parsed data.
 * @return           PARSE_FAIL   - the input text does not contain resource registry data.
 *                   PARSE_OK     - the resource registry data was parsed successfully.
 *                   PARSE_IGNORE - the input text contains trace resource registry, but was
 *                                  set to be ignored by sp_rtrace_parser_set_mask()
 *                                  function.
 */
static int parse_resource_registry(const char* line, sp_rtrace_resource_t* data)
{
	char type[512], desc[512], flags[1024];
	unsigned int id;
	int fields;
	if ( (fields = sscanf(line, "<%x> : %[^ ] (%[^)]) [%[^]]]", &id, type, desc, flags)) < 3) return PARSE_FAIL;
	if ( !(parse_record_mask & SP_RTRACE_RECORD_CONTEXT) ) return PARSE_IGNORE;
	data->id = ffs(id);
	data->type = strdup_a(type);
	data->desc = strdup_a(desc);
	data->flags = fields >= 4 ? parse_resource_flags(flags) : 0;
	return PARSE_OK;
}

/**
 * Parses file attachment record from the input text.
 *
 * @param[in] line   the text to parse.
 * @param[out] data  the parsed data.
 * @return           PARSE_FAIL   - the input text does not contain file attachment data.
 *                   PARSE_OK     - the file attachment data was parsed successfully.
 *                   PARSE_IGNORE - the input text contains trace file attachment, but was
 *                                  set to be ignored by sp_rtrace_parser_set_mask()
 *                                  function.
 */
static int parse_file_attachment(const char* line, sp_rtrace_attachment_t* data)
{
	char name[512], path[PATH_MAX];
	if (sscanf(line, "& %s : %s", name, path) != 2) return PARSE_FAIL;
	if ( !(parse_record_mask & SP_RTRACE_RECORD_ATTACHMENT) ) return PARSE_IGNORE;
	data->name = strdup_a(name);
	data->path = strdup_a(path);
	return PARSE_OK;
}


/*
 * Public API
 */

int sp_rtrace_parser_parse_record(const char* text, sp_rtrace_record_t* record)
{
	int rc;
	rc = parse_backtrace(text, &record->frame);
	if (rc == PARSE_OK) return SP_RTRACE_RECORD_TRACE;
	if (rc == PARSE_IGNORE) return SP_RTRACE_RECORD_NONE;

	rc = parse_function_call(text, &record->call);
	if (rc == PARSE_OK) return SP_RTRACE_RECORD_CALL;
	if (rc == PARSE_IGNORE) return SP_RTRACE_RECORD_NONE;

	rc = parse_arguments(text, &record->arg);
	if (rc == PARSE_OK) return SP_RTRACE_RECORD_ARG;
	if (rc == PARSE_IGNORE) return SP_RTRACE_RECORD_NONE;

	rc = parse_memory_mapping(text, &record->mmap);
	if (rc == PARSE_OK) return SP_RTRACE_RECORD_MMAP;
	if (rc == PARSE_IGNORE) return SP_RTRACE_RECORD_NONE;

	rc = parse_context_registry(text, &record->context);
	if (rc == PARSE_OK) return SP_RTRACE_RECORD_CONTEXT;
	if (rc == PARSE_IGNORE) return SP_RTRACE_RECORD_NONE;

	rc = parse_resource_registry(text, &record->resource);
	if (rc == PARSE_OK) return SP_RTRACE_RECORD_RESOURCE;
	if (rc == PARSE_IGNORE) return SP_RTRACE_RECORD_NONE;

	rc = parse_file_attachment(text, &record->attachment);
	if (rc == PARSE_OK) return SP_RTRACE_RECORD_ATTACHMENT;
	if (rc == PARSE_IGNORE) return SP_RTRACE_RECORD_NONE;

	/* unknown record, assuming it's a comment */
	if ( !(parse_record_mask & SP_RTRACE_RECORD_COMMENT) ) return SP_RTRACE_RECORD_NONE;
	return SP_RTRACE_RECORD_COMMENT;
}



void sp_rtrace_parser_free_record(int type, sp_rtrace_record_t* record)
{
	switch (type) {
		case SP_RTRACE_RECORD_MMAP: {
			if (record->mmap.module) free(record->mmap.module);
			break;
		}
		case SP_RTRACE_RECORD_CALL: {
			if (record->call.name) free(record->call.name);
			if (record->call.res_type_flag == SP_RTRACE_FCALL_RFIELD_NAME && record->call.res_type) free(record->call.res_type);
			break;
		}
		case SP_RTRACE_RECORD_TRACE: {
			if (record->frame.name) free(record->frame.name);
			break;
		}
		case SP_RTRACE_RECORD_ARG: {
			if (record->arg.name) free(record->arg.name);
			if (record->arg.value) free(record->arg.value);
			break;
		}
		case SP_RTRACE_RECORD_CONTEXT: {
			if (record->context.name) free(record->context.name);
			break;
		}
		case SP_RTRACE_RECORD_RESOURCE: {
			if (record->resource.type) free(record->resource.type);
			if (record->resource.desc) free(record->resource.desc);
			break;
		}
		case SP_RTRACE_RECORD_ATTACHMENT: {
			if (record->attachment.name) free(record->attachment.name);
			if (record->attachment.path) free(record->attachment.path);
			break;
		}
	}
}


void sp_rtrace_parser_set_mask(int mask)
{
	parse_record_mask = mask;
}


void sp_rtrace_parser_parse_header(const char* text, sp_rtrace_header_t* header)
{
	char key[256], value[PATH_MAX];
	const char* ptr = text;
	int i;

	memset(header, 0, sizeof(sp_rtrace_header_t));

	while (sscanf(ptr, "%[^=]=%[^,]", key, value) == 2) {
		for (i = 0; i < SP_RTRACE_HEADER_MAX; i++) {
			if (!strcmp(key, header_fields[i])) {
				if (header->fields[i]) free(header->fields[i]);
				header->fields[i] = strdup_a(value);
				break;
			}
		}
		/* advance pointer to the next field */
		ptr = strchr(ptr, ',');
		if (!ptr++) break;
		while (*ptr == ' ') ptr++;
	}
}


void sp_rtrace_parser_free_header(sp_rtrace_header_t* header)
{
	int i;
	for (i = 0; i < SP_RTRACE_HEADER_MAX; i++) {
		if (header->fields[i]) free(header->fields[i]);
	}
}
