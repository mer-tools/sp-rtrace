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

#ifndef SP_RTRACE_H
#define SP_RTRACE_H

/**
 * @file sp_rtrace.h
 *
 * Pre-processor (sp-rtrace) implementation.
 */

#include <signal.h>

/**
 * The pre-processor option structure.
 */
typedef struct rtrace_options_t {
	/* the directory for output files */
	char* output_dir;
	/* true if tracing module must manage the data
	 * pre-processor process by itself */
	bool manage_preproc;
	/* the list of LD_PRELOAD tracing modules */
	char* preload;
	/* the list of LD_AUDIT tracing modules */
	char* audit;
	/* true if the tracing must be enabled at start */
	bool start;
	/* true if toggle signal must be sent also to target's subprocesses */
	bool follow_forks;
	/* the maximum number of frame return addresses in stack trace */
	char* backtrace_depth;
	/* true if the function call (FC) packet timestamps must be disabled */
	bool disable_timestamps;
	/* true if the function argument (FA) packets must be enabled */
	bool enable_arguments;
	/* the post-processor options */
	char* postproc;
	/* signal to use to toggle the tracing */
	char* toggle_signal_name;
	int toggle_signal;
	/* true if event buffering should be disabled */
	bool disable_packet_buffering;
	/* the target process identifier */
	int pid;
	/* the pre-processor work mode */
	int mode;
	/* the post-processor pid */
	int pid_postproc;
	/* the output file name */
	char* output_file;
} rtrace_options_t;

extern rtrace_options_t rtrace_options;

extern sig_atomic_t rtrace_main_loop;

/**
 * Connects output descriptor either to post-processor pipe or
 * binary log file.
 *
 * @return  the opened output file/pipe descriptor.
 */
int rtrace_connect_output();

#endif

