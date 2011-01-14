/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2011 by Nokia Corporation
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

/**
 * @file sp_rtrace_pagemap.c
 *
 * Module to trace empty pagemap pages ((libsp-rtrace-pagemap.so).
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <unistd.h>
#include <pthread.h>

#include "sp_rtrace_main.h"
#include "sp_rtrace_module.h"
#include "common/sp_rtrace_proto.h"
#include "common/sp_rtrace_defs.h"
#include "common/debug_log.h"
#include "common/utils.h"


//#define MSG(text) {	char buffer[] = ">>>" text "\n"; if(write(STDERR_FILENO, buffer, sizeof(buffer))){}; }

/* Module information */
static sp_rtrace_module_info_t module_info = {
		.type = MODULE_TYPE_PRELOAD,
		.version_major = 1,
		.version_minor = 0,
		.name = "pagemap",
		.description = "Empty pagemap pages tracking module. "
				       "Tracks allocated pages containing only zero bytes and attaches page mapping data "
				       "from /proc/pid/pagemap and /proc/pageflags.",
};

static bool trace_enabled = false;

/**
 * Enables/disables tracing.
 *
 * @param[in] value   enable tracing if true, otherwise disable it.
 * @return
 */
static void enable_tracing(bool value)
{
	if (!value && trace_enabled) {
		char filename[PATH_MAX];
		sp_rtrace_get_out_filename("pg-map", filename, sizeof(filename));
		sp_rtrace_attachment_t file = {
				.name = "test",
				.path = filename,
		};
		sp_rtrace_copy_file("/proc/self/maps", filename);
		sp_rtrace_write_attachment(&file);
	}
	trace_enabled = value;
}


static void trace_pagemap_init(void) __attribute__((constructor));
static void trace_pagemap_fini(void) __attribute__((destructor));


static void trace_pagemap_init(void)
{
	sp_rtrace_register_module(module_info.name, module_info.version_major, module_info.version_minor, enable_tracing);
}

static void trace_pagemap_fini(void)
{
	enable_tracing(false);
	LOG("fini");
}

/**
 * Gets module information data.
 *
 * @return  the module information data.
 */
const sp_rtrace_module_info_t* sp_rtrace_get_module_info()
{
	return &module_info;
}
