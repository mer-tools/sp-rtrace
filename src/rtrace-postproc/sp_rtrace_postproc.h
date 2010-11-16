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

#ifndef  SP_RTRACE_POSTPROC_H
#define SP_RTRACE_POSTPROC_H

/**
 * @file sp_rtrace_postproc.h
 *
 * The post-processor implementation.
 */

#include <stdbool.h>
#include <signal.h>

#include "common/data_ops.h"

/**
 * post-processor options
 */
typedef struct postproc_options_t {
	char* input_file;
	char* output_dir;
	bool filter_leaks;
	bool compress;
	bool remove_args;
	bool resolve;
	int filter_context;
	int filter_resource;
	op_binary_t compare_leaks;
	int pid_resolve;
	int backtrace_depth;
} postproc_options_t;

extern postproc_options_t postproc_options;

extern volatile sig_atomic_t postproc_abort;

#endif
