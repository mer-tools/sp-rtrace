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

#ifndef SP_RTRACE_DEFS_H
#define SP_RTRACE_DEFS_H

/**
 * @file sp_rtrace_defs.h
 *
 * Common definitions used by sp-strace protocols.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Resource behavior flags
 */
enum sp_rtrace_resource_flags_t {
	/* Resource alloc/free functions uses reference counting. This means that
	 * the subsequent allocation functions for the resource with the same resource 
	 * id increments the resource reference counter. Free functions decrements 
	 * the reference counter and the resource is assumed to be freed after the
	 * reference counter reaches zero. */
	SP_RTRACE_RESOURCE_REFCOUNT = 1 << 0,
	
	/* The last resource behavior flag */
	SP_RTRACE_RESOURCE_LAST_FLAG = SP_RTRACE_RESOURCE_REFCOUNT, 
			
	/* Default resource behavior flags */
	SP_RTRACE_RESOURCE_DEFAULT = 0,

	/* the number of resource flags */
	SP_RTRACE_RESOURCE_FLAGS_MAX = 1,
 };

/**
 * Function call types.
 */
enum sp_rtrace_fcall_type_t {
	SP_RTRACE_FTYPE_UNDEF = 0,//
	SP_RTRACE_FTYPE_FREE = 1, // allocation function call
	SP_RTRACE_FTYPE_ALLOC = 2,// deallocation (free) function call
};


/**
 * Pointer data type used to store addresses.
 */
typedef unsigned long pointer_t;


/**
 * Function call data structure.
 */
typedef struct sp_rtrace_fcall_t {
	/* the function index */
	int index;
	/* the function call type (sp_rtrace_fcall_type_t structure) */
	unsigned int type;
	/* the function call context */
	unsigned int context;
	/* the function call timestamp */
	unsigned int timestamp;
	/* the function name */
	char* name;
	/* the resource type. The meaning of the res_type field depends
	 * on the res_type_flag field */
	void* res_type;
	/* Defines the res_type field type. See sp_rtrace_fcall_rfield_t enum */
	unsigned int res_type_flag;
	/* the associated (allocated/freed) resource identifier */
	pointer_t res_id;
	/* the associated (allocated) resource size */
	int res_size;
} sp_rtrace_fcall_t;


/**
 * The sp_rtrace_fcall_t res_type field type.
 */
enum sp_rtrace_fcall_rfield_t {
	SP_RTRACE_FCALL_RFIELD_UNDEF = 0,//
	SP_RTRACE_FCALL_RFIELD_REF = 1,  // the field contains reference to the internal resource
	                                 // data structure.
	SP_RTRACE_FCALL_RFIELD_ID = 2,   // the field contains resource type id.
	SP_RTRACE_FCALL_RFIELD_NAME = 3, // the field contains resource type name.
};


/**
 * Header field index values.
 *
 * Also see the TEXT_PROTOCOL file for the header field
 * description.
 */
enum sp_rtrace_header_index_t {
	/* the protocol version, set internally by sp-rtrace */
	SP_RTRACE_HEADER_VERSION,
	/* the architecture of the report source system */
	SP_RTRACE_HEADER_ARCH,
	/* the report creation timestamp, set by sp-rtrace if left NULL */
	SP_RTRACE_HEADER_TIMESTAMP,
	/* the tracked process name */
	SP_RTRACE_HEADER_PROCESS,
	/* the tracked process identifier */
	SP_RTRACE_HEADER_PID,
	/* the filters, applied to the report */
	SP_RTRACE_HEADER_FILTER,
	/* the maximum depth of the function stack trace */
	SP_RTRACE_HEADER_BACKTRACE_DEPTH,
	/* the initial report generator tool */
	SP_RTRACE_HEADER_ORIGIN,

	/* the number of header fields */
	SP_RTRACE_HEADER_MAX,
};

/**
 * Header data structure.
 *
 * The header data structure consists of a set of predefined
 * fields. See sp_rtrace_header_index_t enum.
 */
typedef struct sp_rtrace_header_t {
	char* fields[SP_RTRACE_HEADER_MAX];
} sp_rtrace_header_t;

/**
 * Memory mapping data.
 */
typedef struct sp_rtrace_mmap_t {
	pointer_t from;
	pointer_t to;
	char* module;
} sp_rtrace_mmap_t;


/**
 * Function stack trace data
 */
typedef struct sp_rtrace_ftrace_t {
	/* number of frames in stack trace */
	unsigned long nframes;

	/* the frame return addresses */
	pointer_t* frames;

	/* The resolved address names. NULL when parsing binary data,
	 * but text files could contain already resolved names -
	 * in this case keep them intact. */
	char** resolved_names;
} sp_rtrace_ftrace_t;


/**
 * Call context data.
 */
typedef struct sp_rtrace_context_t {
	/* the context id */
	unsigned long id;
	/* the context name */
	char* name;
} sp_rtrace_context_t;

/**
 * Resource type information.
 */
typedef struct sp_rtrace_resource_t {
	/* the resource type id */
	unsigned long id;
	/* the resource type */
	char* type;
	/* the resource description */
	char* desc;
	/* resource behaviour flags */
	unsigned int flags;
} sp_rtrace_resource_t;

/**
 * Function argument data
 */
typedef struct sp_rtrace_farg_t {
	/* the function argument name. Usually it matches the function argument prototype name */
	char* name;
	/* the function argument value. */
	char* value;
} sp_rtrace_farg_t;

/**
 * File attachment data
 */
typedef struct sp_rtrace_attachment_t {
	/* the attachment name */
	char* name;
	/* name of the attached file */
	char* path;
} sp_rtrace_attachment_t;


/**
 * Single backtrace step frame data.
 */
typedef struct {
	/* the call address */
	pointer_t addr;
	/* resolved caller function name */
	char* name;
} sp_rtrace_btframe_t;

#ifdef  __cplusplus
}
#endif

#endif
