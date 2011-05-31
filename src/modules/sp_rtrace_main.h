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

#ifndef SP_RTRACE_MAIN_H
#define SP_RTRACE_MAIN_H

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <limits.h>

#include "common/sp_rtrace_proto.h"
#include "library/sp_rtrace_defs.h"
#include "library/sp_rtrace_filter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file sp_rtrace_main.h
 *
 * Main tracing module (libsp-rtrace-main.so) implementation.
 *
 * This module is responsible for:
 * 1) event processor named pipe management (including
 *    staring event processor if necessary,
 * 2) sending handshake and module packets to processor,
 * 3) providing API for sending rest of the packets to
 *    processor,
 * 4) managing local buffer to cache writes to processor
 *    pipe,
 * 5) tracing enabling/disabling by SIGUSR1 signal,
 */

/**
 * Tracing options.
 */
typedef struct sp_rtrace_options_t {
	int backtrace_depth;
	bool enable_timestamps;
	bool enable_ext_args;
	bool enable;
	bool manage_preproc;
	bool enable_packet_buffering;
	char output_dir[PATH_MAX];
	char postproc[PATH_MAX];
	sp_rtrace_filter_t* filter;
} sp_rtrace_options_t;

extern sp_rtrace_options_t* sp_rtrace_options;

/**
 * Writes function call packet into processor pipe.
 *
 * @param[in] call  the function call data.
 * @param[in] args  the function argument data (can be NULL).
 * @return          the number of bytes written.
 */
int sp_rtrace_write_function_call(sp_rtrace_fcall_t* call, sp_rtrace_ftrace_t* trace, sp_rtrace_farg_t* args);


typedef void (*sp_rtrace_enable_tracing_t)(bool);

/**
 * Registers tracing sub module.
 *
 * Each module (except the main) should call this function to register its enable/disable
 * function.
 * The main module uses registered functions to disable tracing in all modules before
 * calling some libc functions (for example backtrace). Or some functions might end
 * in infinite recursion.
 * @param[in] name          the module name.
 * @param[in] vmajor        the module version major number.
 * @param[in] vminor        the module version minor number.
 * @param[in] enable_func   the trace enabling/disabling function.
 * @return                  the module id or 0 if module registry is full.
 */
unsigned int sp_rtrace_register_module(const char* name, unsigned char vmajor, unsigned char vminor,
		sp_rtrace_enable_tracing_t enable_func);


/**
 * Registers resource type.
 *
 * During initialization phase every module must register resources
 * it's going to track. This allows to identify the resource types
 * when multiple modules are used or a single module tracks multiple
 * resource types.
 * Note that the resource type and desc values must refer to preallocated
 * strings (either static or dynamic), which must not be freed until the module
 * is unloaded.
 * @param[in,out] resource in  - the resource data.
 *                         out - the resource identifier.
 * @return                 the resource type id or 0 if resource registry is full.
 */
unsigned int sp_rtrace_register_resource(sp_rtrace_resource_t* resource);

/**
 * Stores current heap information (mallinfo()) so it can be sent to pre-processor
 * when tracing is disabled.
 *
 * @return
 */
void sp_rtrace_store_heap_info();

/**
 * Writes context registry packet into processor pipe.
 *
 * @param[in] context     the context data.
 * @return                the number of bytes written.
 */
int sp_rtrace_write_context_registry(sp_rtrace_context_t* context);


/**
 * Initializes the tracing.
 *
 * @return   true - the initialization was successful.
 *           false - the initialization failed.
 */
bool sp_rtrace_initialize();

/**
 * Writes new library (NL) packet into processor pipe.
 *
 * @param[in] library   the library name.
 * @return              the number of bytes written.
 */
int sp_rtrace_write_new_library(const char* library);


/**
 * Write file attachment packet into processor pipe.
 *
 * @param[in] file     the attachment data.
 * @return             the number of bytes written.
 */
int sp_rtrace_write_attachment(const sp_rtrace_attachment_t* file);

/**
 * Generates unique filename in the output directory.
 *
 * The filename starts with the specified pattern and an incremental
 * index is added until new filename is found.
 * @param[in] pattern  the name pattern.
 * @param[out] buffer  the output buffer.
 * @param[in] size     the output buffer size.
 */
void sp_rtrace_get_out_filename(const char* pattern, char* buffer, size_t size);

/**
 * Copies file.
 *
 * @param[in] source  the source file path.
 * @param[in] dest    the destination file path.
 * @return             0 - success
 *                    <0 - -errno
 */
int sp_rtrace_copy_file(const char* source, const char* dest);


#ifdef __cplusplus
extern "C" }
#endif


#endif
