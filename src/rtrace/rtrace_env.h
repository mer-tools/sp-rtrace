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

#ifndef RTRACE_ENV_H
#define RTRACE_ENV_H

/**
 * @file rtrace_env.h
 *
 * Contains definitions of (sp-rtrace) options and environment
 * variables.
 *
 * Those options are used to
 * 1) parse sp-rtrace command line arguments,
 * 2) setup environment variables for tracing process launching,
 * 3) create pre-processor option list from the environment variables
 *    (used by main trace module in managed mode).
 */

#include <getopt.h>
#include <stdlib.h>

#define OPT_ENABLE    "1"

/**
 * pre-processor(sp-rtrace) option index.
 */
enum {
	OPT_INPUT_FILE,              //!< OPT_INPUT_FILE
	OPT_OUTPUT_DIR,              //!< OPT_OUTPUT_DIR
	OPT_MANAGE_PREPROC,          //!< OPT_MANAGE_PREPROC
	OPT_PRELOAD,                 //!< OPT_PRELOAD
	OPT_AUDIT,                   //!< OPT_AUDIT
	OPT_START,                   //!< OPT_START
	OPT_TOGGLE,                  //!< OPT_TOGGLE
	OPT_FOLLOW_FORKS,            //!< OPT_FOLLOW_FORKS
	OPT_BACKTRACE_DEPTH,         //!< OPT_BACKTRACE_DEPTH
	OPT_DISABLE_TIMESTAMPS,      //!< OPT_DISABLE_TIMESTAMPS
	OPT_ENABLE_ARGUMENTS,        //!< OPT_ENABLE_ARGUMENTS
	OPT_POSTPROC,                //!< OPT_POSTPROC
	OPT_TOGGLE_SIGNAL,           //!< OPT_TOGGLE_SIGNAL
	OPT_DISABLE_PACKET_BUFFERING,//!< OPT_DISABLE_PACKET_BUFFERING
	MAX_OPT                      //!< MAX_OPT
};

/**
 * long pre-processor options
 */
extern struct option rtrace_long_opt[];

/**
 * environment variables, representing pre-processor options.
 */
extern char *rtrace_env_opt[];

/**
 * short pre-processor options
 */
extern char* rtrace_short_opt;

/* The maximum total size of all command line arguments */
#define RTRACE_MAX_ARGS_SIZE        4096

/**
 * Command line argument structure, used for execv function calls.
 *
 */
typedef struct rtrace_args_t {
	/* the command line arguments separated with NUL character */
	char args[RTRACE_MAX_ARGS_SIZE];
	/* references of the command line arguments in args array */
	char* argv[MAX_OPT + 1];
	/* reference to the first free character in the args array */
	char* head;
	/* number of arguments stored */
	int index;
} rtrace_args_t;

/**
 * Converts rtrace environment options to the pre-processor
 * command line arguments.
 * @param[in] args   the assembled argument list.
 * @param[in] app     the application name (must be the first argument).
 */
int rtrace_args_scan_env(rtrace_args_t* args, const char* app);

/**
 * Append an option to the end of argument list.
 *
 * @param[in] args    the argument list.
 * @param[in] opt     the option to append.
 * @param[in] value   the option value (can be NULL).
 * @return            0 - success.
 */
void rtrace_args_add_opt(rtrace_args_t* args, char opt, const char* value);

/**
 * Finishes argument list by appending trailing NULL.
 *
 * @param[in] args  the argument list.
 * @return
 */
void rtrace_args_finish(rtrace_args_t* args);

#endif

