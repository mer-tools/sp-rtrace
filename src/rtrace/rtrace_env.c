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

#include <string.h>
#include <stdio.h>

#include "rtrace_env.h"
#include "common/utils.h"

/**
 * pre-processor long option list
 */
struct option rtrace_long_opt[] = {
		 {"input-file", 1, 0, 'i'},
		 {"output-dir", 1, 0, 'o'},
		 {"manage-preproc", 0, 0, 'm'},
		 {"preload", 1, 0, 'p'},
		 {"audit", 1, 0, 'a'},
		 {"start", 0, 0, 's'},
		 {"toggle", 1, 0, 't'},
		 {"follow-forks", 0, 0, 'f'},
		 {"backtrace-depth", 1, 0, 'b'},
		 {"disable-timestamps", 0, 0, 'T'},
		 {"postproc", 1, 0, 'P'},
		 {"toggle-signal", 1, 0, 'S'},
		 {"disable-packet-buffering", 0, 0, 'B'},
		 {"execute", 1, 0, 'x'},
		 {"list", 0, 0, 'l'},
		 {"listen", 0, 0, 'L'},
		 {"help", 0, 0, 'h'},
		 {"backtrace-all", 0, 0, 'A'},
		 {"libunwind", 0, 0, 'u'},
		 {0, 0, 0, 0}
};

/**
 * pre-processor environment variable list.
 * The environment variables must have the same index as
 * related option index.
 */
char *rtrace_env_opt[] = {
		/**
		 * --input-file
		 * Specifies the output directory for rtrace logs.
		 * By default the home directory is used.
		 */
		"SP_RTRACE_INPUT_FILE",
		/**
		 * --output-dir
		 * Specifies the output directory for rtrace logs.
		 * By default the home directory is used.
		 */
		"SP_RTRACE_OUTPUT_DIR",
		/**
		 * --manage-preproc
		 * Defines that the main trace module must spawn pre-processor process when necessary.
		 * By default sp-rtrace is spawned externally and the main module just connects to its named pipe.
		 */
		"SP_RTRACE_MANAGE_PREPROC",
		/**
		 * --preload
		 * List of LD_PRELOAD tracing modules, separated by ':'
		 */
		"SP_RTRACE_PRELOAD",
		/**
		 * --audit
		 * List of LD_AUDIT tracing modules, separated by ':'
		 */
		"SP_RTRACE_AUDIT",
		/**
		 * --start
		 * Defines that trace should be started automatically when the target process is spawned.
		 * By default rtace is started in disabled mode and toggle signal must be sent to the
		 * target process to start tracing.
		 */
		"SP_RTRACE_START",
		/**
		 * --toggle
		 */
		NULL,
		/**
		 * --follow-forks
		 */
		NULL,
		/**
		 * --backtrace-depth
		 *  Defines the default number of frame return addresses in backtrace.
		 *  By default 10 frames are retrieved.
		 */
		"SP_RTRACE_BACKTRACE_DEPTH",
		/**
		 * --disable-timestamps
		 * Disables the timestamps for FC packets.
		 * By default the timestamps are enabled.
		 */
		"SP_RTRACE_DISABLE_TIMESTAMPS",
		/**
		 * --postproc
		 * Specifies post-processing options.
		 * This option is used when SP_RTRACE_MANAGE_PREPROC is set.
		 */
		"SP_RTRACE_POSTPROC",
		/**
		 * --toggle-signal
		 * Specifies the trace toggling signal number.
		 * By default SIGUSR1 is used.
		 */
		"SP_RTRACE_TOGGLE_SIGNAL",
		/**
		 * --disable-packet-buffering
		 * Disables the main tracing module internal packet buffer causing all
		 * data to be written directly to the pre-processor pipe.0
		 */
		"SP_RTRACE_DISABLE_PACKET_BUFFERING",
		/**
		 * --execute
		 */
		NULL,
		/**
		 * --list
		 */
		NULL,
		/**
		 * --listen
		 */
		NULL,
		/**
		 * --help
		 */
		NULL,
		/**
		 * --backtrace-all
		 */
		"SP_RTRACE_BACKTRACE_ALL",
		/**
		 * --libunwind
		 */
		"SP_RTRACE_LIBUNWIND",
		/**
		 * Trailing NULL
		 */
		NULL,
};

/* sp_rtrace short option list */
char* rtrace_short_opt = "i:o:mp:a:st:fb:TAP:S:Bhx:lLFu";

void rtrace_args_add_opt(rtrace_args_t* args, char opt, const char* value)
{
	args->argv[args->index++] = args->head;
	*args->head++ = '-';
	*args->head++ = opt;
	if (value) {
		int len = strlen(value);
		memcpy(args->head, value, len);
		args->head += len;
	}
	*args->head++ = '\0';
}

int rtrace_args_scan_env(rtrace_args_t* args, const char* app)
{
	/* initialize args structure and store the first argument - process name */
	args->head = args->args;
	args->argv[0] = args->args;
	args->index = 1;
	int len = strlen(app) + 1;
	memcpy(args->head, app, len);
	args->head += len;

	/* process option list */
	int i;
	for (i = 0; i < MAX_OPT; i++) {
		if (!rtrace_env_opt[i]) continue;
		const char* env = getenv(rtrace_env_opt[i]);
		if (env) {
			struct option* opt = &rtrace_long_opt[i];
			rtrace_args_add_opt(args, opt->val, opt->has_arg ? env : NULL);
		}
	}
	return args->index;
}

void rtrace_args_finish(rtrace_args_t* args)
{
	args->argv[args->index] = NULL;
}


