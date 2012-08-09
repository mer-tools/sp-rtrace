/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010-2012 by Nokia Corporation
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

#ifndef RTRACE_DATA_H
#define RTRACE_DATA_H

/**
 * @file rtrace_data.h
 *
 * Contains definitions of data structures used
 * to store the resource trace data.
 * It also contains and support (but not parsing) functions.
 *
 * The parsing functions are defined respectively in the
 * parse-binary.h and parse-text.h files.
 */

#include <stdbool.h>
#include <sys/time.h>

#include "htable.h"
#include "dlist.h"
#include "sp_rtrace_proto.h"
#include "library/sp_rtrace_defs.h"


/**
 * The handshake data structure.
 *
 * Used to store handshake information (HS packet).
 */
typedef struct rd_hshake_t {
	unsigned char vmajor;
	unsigned char vminor;
	char* arch;
	unsigned char endianness;
	unsigned char pointer_size;
} rd_hshake_t;

#define RD_HSHAKE(x)	((rd_hshake_t*)x)

#define HS_CHECK_VERSION(hs, major, minor) (hs->vmajor > major || (hs->vmajor == major && hs->vminor >= minor))


/**
 * Frees handshake data.
 *
 * @param[in] hs   the data to free.
 * @return
 */
void rd_hashake_free(rd_hshake_t* hs);

/**
 * Process information data structure.
 *
 * Used to store PI packet.
 */
typedef struct rd_pinfo_t {
	unsigned int pid;
	unsigned char flags;
	int backtrace_depth;
	char* name;
	struct timeval timestamp;
	char* trace_origin;
} rd_pinfo_t;

#define RD_PINFO(x) ((rd_pinfo_t*)x)

/**
 * Frees process information data.
 *
 * @param[in] pinfo   the data to free.
 * @return
 */
void rd_pinfo_free(rd_pinfo_t* pinfo);


/**
 * Module information data structure.
 *
 * Used to store MI packet.
 */
typedef struct rd_minfo_t {
	/* double linked list support */
	dlist_node_t node;
	/* module id */
	unsigned int id;
	/* module information record data */
	unsigned char vmajor;
	unsigned char vminor;
	char* name;
} rd_minfo_t;

#define RD_MINFO(x) ((rd_minfo_t*)x)

/**
 * Frees module information data.
 *
 * @param[in] minfo   the data to free.
 * @return
 */
void rd_minfo_free(rd_minfo_t* minfo);

/**
 * Module memory mapping data structure.
 *
 * Used to store MM packet.
 */
typedef struct rd_mmap_t {
	dlist_node_t node;

	sp_rtrace_mmap_t data;
} rd_mmap_t;

#define RD_MMAP(x) ((rd_mmap_t*)x)

/**
 * Frees module memory mapping data.
 *
 * @param[in] mmap   the data to free.
 * @return
 */
void rd_mmap_free(rd_mmap_t* mmap);


/**
 * Context registry data structure.
 *
 * Used to store CR packet.
 */
typedef struct rd_context_t {
	/* double linked list support */
	dlist_node_t node;

	sp_rtrace_context_t data;
} rd_context_t;

#define RD_CONTEXT(x) ((rd_context_t*)x)

/**
 * Frees context registry data.
 *
 * @param[in] context   the data to free.
 * @return
 */
void rd_context_free(rd_context_t* context);


/**
 * Resource registry data structure.
 *
 * Used to store RR packet.
 */
typedef struct rd_resource_t {
	/* double linked list support */
	dlist_node_t node;

	/* the resource data */
	sp_rtrace_resource_t data;
	/* flag to hide resource from call records */
	bool hide;
} rd_resource_t;

#define RD_RESOURCE(x) ((rd_resource_t*)x)

/**
 * Frees context registry data.
 *
 * @param[in] context   the data to free.
 * @return
 */
void rd_resource_free(rd_resource_t* resource);


/**
 * Function call data.
 *
 * Used to store FC and optionally FA packets.
 * Also contains link to the related function trace
 * data (BT packet).
 */
typedef struct rd_fcall_t {
	/* double linked list support */
	dlist_node_t node;

	/* the function call data */
	sp_rtrace_fcall_t data;

	/* the function call backtrace */
	struct rd_ftrace_t* trace;

	/* back reference from ftrace record */
	ref_node_t* ref;

	/* the function call arguments */
	struct rd_fargs_t* args;
} rd_fcall_t;

#define RD_FCALL(x) ((rd_fcall_t*)x)

#define RD_FCALL_RESOURCE(x) (((rd_fcall_t*)x)->res_type_flag == SP_RTRACE_FCALL_RFIELD_REF ? ((rd_fcall_t*)x)->res_type : NULL)

/**
 * Frees function call data.
 *
 * @param[in] call   the data to free.
 * @return
 */
void rd_fcall_free(rd_fcall_t* call);


/**
 * The backtrace data.
 *
 * Single backtrace data structure can be shared between multiple
 * functions calls structures.
 * Used to store BT packet.
 */
typedef struct rd_ftrace_t {
	/* hash table support */
	htable_node_t node;

	/* the reference counter */
	int ref_count;

	/* the backtrce data */
	sp_rtrace_ftrace_t data;

	/* list of function calls associated with the backtrace */
	dlist_t calls;

} rd_ftrace_t;

#define RD_FTRACE(x) ((rd_ftrace_t*)x);

/**
 * Frees function trace data.
 *
 * @param[in] trace   the data to free.
 * @return
 */
void rd_ftrace_free(rd_ftrace_t* trace);

/**
 * The function arguments record.
 *
 * Contains zero terminated array of function argument values
 * in text format.
 */
typedef struct rd_fargs_t {
	sp_rtrace_farg_t* data;
} rd_fargs_t;

#define RD_FARGS(x) ((rd_fargs_t*)x)

/**
 * Frees function arguments data.
 *
 * @param[in] args  the data to free.
 * @return
 */
void rd_fargs_free(rd_fargs_t* args);


/**
 * The comment record.
 *
 * Comment records contains all unrecognized text lines.
 * The post processor tries its best to keep the relative
 * position of comments the same. If a comment was found
 * before a function in input file, it will also be before
 * the same function in output file.
 */
typedef struct rd_comment_t {
	/* double linked list support */
	dlist_node_t node;
	/* index of previous function call record */
	int index;
	/* the comment text */
	char* text;
} rd_comment_t;

#define RD_COMMENT(x) ((rd_comment_t*)x)

/**
 * Frees comment data.
 *
 * @param[in] comment  the data to free.
 * @return
 */
void rd_comment_free(rd_comment_t* comment);

/**
 * Heap information record.
 */
typedef struct {
	pointer_t heap_bottom;
	pointer_t heap_top;
	pointer_t lowest_block;
	pointer_t highest_block;

	int arena;
	int ordblks;
	int smblks;
	int hblks;
	int hblkhd;
	int usmblks;
	int fsmblks;
	int uordblks;
	int fordblks;
	int keepcost;

} rd_hinfo_t;

#define RD_HINFO(x) ((rd_hinfo_t*)x)

/**
 * Frees heap information data.
 *
 * @param[in] hinfo  the data to free.
 * @return
 */
void rd_hinfo_free(rd_hinfo_t* hinfo);


typedef struct {
	dlist_node_t node;
	sp_rtrace_attachment_t data;
} rd_attachment_t;

void rd_attachment_free(rd_attachment_t* attachment);
/*
 *
 */

/**
 * Resource trace data.
 *
 * This structure contains all data found in the
 * resource trace file (binary or text).
 */
typedef struct rd_t {
	/* function call records */
	dlist_t calls;
	/* context registry */
	dlist_t contexts;
	/* function call backtraces */
	htable_t ftraces;
	/* memory mapping information */
	dlist_t mmaps;
	/* handshake record */
	rd_hshake_t* hshake;
	/* process info record */
	rd_pinfo_t* pinfo;
	/* module info records */
	dlist_t minfo;
	/* comment data */
	dlist_t comments;
	/* heap information data (optional) */
	rd_hinfo_t* hinfo;
	/* resource registry */
	dlist_t resources;
	/* mask of applied filters */
	unsigned int filter;
	/* list of attached files */
	dlist_t files;
} rd_t;


/**
 * Creates and initializes resource trace data structure.
 *
 * @return   the new resource trace data object.
 */
rd_t* rd_create(void);

/**
 * Frees trace data.
 *
 * @param[in] data   the trace data.
 * @return
 */
void rd_free(rd_t* data);


/**
 * Removes function call data.
 *
 * This function removes function call data from the function call
 * list and properly releases the associated resources (backtrace,
 * arguments).
 * The function call data is freed afterwards.
 * @param[in] data   the trace data.
 * @param[in] call   the function call record.
 */
void rd_fcall_remove(rd_t* rd, rd_fcall_t* call);

/**
 * Assigns backtrace data to function call data.
 *
 * This function checks if the backtrace is already registered in
 * backtrace table. If yes - the existing record is reused and the
 * @p trace is freed. Otherwise @p trace backtrace is stored into
 * backtrace table.
 * @param[in] rd     the resource trace data.
 * @param[in] call   the function call.
 * @param[in] trace  the backtrace data.
 * @return
 */
void rd_fcall_set_ftrace(rd_t* rd, rd_fcall_t* call, rd_ftrace_t* trace);

/**
 * Assigns backtrace data to function calls.
 *
 * This function checks if the backtrace is already registered in
 * backtrace table. If yes - the existing record is reused and the
 * @p trace is freed. Otherwise @p trace backtrace is stored into
 * backtrace table.
 * @param[in] rd     the resource trace data.
 * @param[in] calls  the function call reference list.
 * @param[in] trace  the backtrace data.
 * @return
 */
void rd_fcalls_set_ftrace(rd_t* rd, dlist_t* calls, rd_ftrace_t* trace);

#endif

