/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2011 by Nokia Corporation
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
 * @file sp_rtrace_parser.h
 *
 * Parser API provides functionality to parse sp-rtrace text format files.
 *
 * Usage:
 * 1) Use sp_rtrace_parser_set_mask() function to choose the records
 *    to parse. By default all records are parsed, however user can
 *    select only the record types he is interested in, slightly
 *    increasing the parsing speed.
 *
 * 2) Use sp_rtrace_parser_parse_header() function to parse the report
 *    header, which always is the first line in the report.
 *    This function allocates resources for header field value storage
 *    so the caller is responsible for freeing them later.
 *
 * 3) Use sp_rtrace_parser_parse_record() function to parse the rest of
 *    the report lines. This function returns the parsed record type.
 *    SP_RTRACE_RECORD_NONE is returned if the line contains record that
 *    is set to be ignored (with sp_rtrace_parser_set_mask()). This function
 *    might allocate data for record specific resource storage (comment
 *    record is an exception, as the line itself is the comment data).
 *    User is responsible for handling the allocated resources manually
 *    or by freeing them with sp_rtrace_parser_free_record() function.
 *
 */

#ifndef _SP_RTRACE_PARSER_H_
#define _SP_RTRACE_PARSER_H_

#include "sp_rtrace_defs.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Record types returned by sp_rtrace_parse_record() function.
 */
typedef enum {
	SP_RTRACE_RECORD_NONE         = 0,     //!< SP_RTRACE_RECORD_UNKNOWN
	SP_RTRACE_RECORD_MMAP         = 1 << 0,//!< SP_RTRACE_RECORD_MMAP
	SP_RTRACE_RECORD_COMMENT      = 1 << 1,//!< SP_RTRACE_RECORD_COMMENT
	SP_RTRACE_RECORD_CALL         = 1 << 2,//!< SP_RTRACE_RECORD_CALL
	SP_RTRACE_RECORD_TRACE        = 1 << 3,//!< SP_RTRACE_RECORD_TRACE
	SP_RTRACE_RECORD_ARG          = 1 << 4,//!< SP_RTRACE_RECORD_ARG
	SP_RTRACE_RECORD_CONTEXT      = 1 << 5,//!< SP_RTRACE_RECORD_CONTEXT
	SP_RTRACE_RECORD_RESOURCE     = 1 << 6,//!< SP_RTRACE_RECORD_RESOURCE
	SP_RTRACE_RECORD_ATTACHMENT   = 1 << 7,//!< SP_RTRACE_RECORD_ATTACH

	SP_RTRACE_RECORD_ALL       = 0xFFFF,//!< SP_RTRACE_RECORD_ALL
} sp_rtrace_record_type_t;


/**
 * The data structure used to store sp_rtrace_parse_record() output.
 */
typedef union {
	/* data of SP_RTRACE_RECORD_MMAP record type */
	sp_rtrace_mmap_t mmap;
	/* data of SP_RTRACE_RECORD_CALL record type */
	sp_rtrace_fcall_t call;
	/* data of SP_RTRACE_RECORD_TRACE record type */
	sp_rtrace_btframe_t frame;
	/* data of SP_RTRACE_RECORD_ARG record type */
	sp_rtrace_farg_t arg;
	/* data of SP_RTRACE_RECORD_CONTEXT record type */
	sp_rtrace_context_t context;
	/* data of SP_RTRACE_RECORD_RESOURCE record type */
	sp_rtrace_resource_t resource;
	/* data of SP_RTRACE_RECORD_ATTACHMENT record type */
	sp_rtrace_attachment_t attachment;
} sp_rtrace_record_t;


/**
 * Parses the text containing sp-rtrace text format record.
 *
 * The data is stored in the sp_rtrace_record_t union depending on
 * the record type (see the sp_rtrace_record_t union for record type-
 * union member relations).
 * This function might allocate record type specific data, which must
 * be freed with sp_rtrace_free_record() function if not stored anywhere.
 * @param[in] text      the text to parse.
 * @param[out] record   the parsed data.
 * @return              the record type (see sp_rtrace_record_type_t enum).
 */
int sp_rtrace_parser_parse_record(const char* text, sp_rtrace_record_t* record);

/**
 * Frees the sp-rtrace record data parsed by sp_rtrace_parse_record() function.
 *
 * @param[in] type    the record type (see sp_rtrace_record_type_t enum).
 * @param[in] record  the record to free.
 */
void sp_rtrace_parser_free_record(int type, sp_rtrace_record_t* record);

/**
 * Sets mask of the record types to parse (see the sp_rtrace_record_type_t enum).
 *
 * By default all record types are parsed (the mask is SP_RTRACE_RECORD_ALL).
 * @param[in] mask  the record type mask/
 */
void sp_rtrace_parser_set_mask(int mask);


/**
 * Parses text into header fields.
 *
 * The header fields are stored in the sp_rtrace_header_t structure accordingly
 * to their index (see the sp_rtrace_header_index_t enum). The caller is responsible
 * for managing the memory allocated for field values (either copy contents and
 * release the header with sp_rtrace_parser_free_header() function or store the
 * field references and release them separately).
 * @param[in] text     the text to parse
 * @param[in] header   the header data.
 * @return             0 - success.
 */
void sp_rtrace_parser_parse_header(const char* text, sp_rtrace_header_t* header);


/**
 * Frees the header resources.
 *
 * @param[in] header   the header to free.
 */
void sp_rtrace_parser_free_header(sp_rtrace_header_t* header);

#ifdef  __cplusplus
}
#endif

#endif
