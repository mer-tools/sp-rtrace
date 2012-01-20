/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010-2012 by Nokia Corporation
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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "common/sp_rtrace_proto.h"
#include "common/rtrace_data.h"
#include "common/utils.h"
#include "common/msg.h"

#include "rtrace_common.h"

#include "sp_rtrace_postproc.h"
#include "parse_binary.h"
#include "parse_text.h"
#include "leaks_sort.h"
#include "filter.h"
#include "writer.h"

/**
 * The post-processor options.
 */
postproc_options_t postproc_options = {
	.input_file = NULL,
	.output_dir = NULL,
	.filter_leaks = false,
	.compress = false,
	.remove_args = false,
	.resolve = false,
	.filter_context = -1,
	.compare_leaks = 0,
	.pid_resolve = 0,
	.filter_resource = 0,
	.backtrace_depth = -1,
	.include_file = NULL,
	.exclude_file = NULL,
};

volatile sig_atomic_t postproc_abort = 0;

/**
 * Free options resources allocated during command line parsing.
 *
 * @return
 */
static void free_options()
{
	if (postproc_options.input_file) free(postproc_options.input_file);
	if (postproc_options.output_dir) free(postproc_options.output_dir);
	if (postproc_options.include_file) free(postproc_options.include_file);
	if (postproc_options.exclude_file) free(postproc_options.exclude_file);
}

/**
 * Display the help information.
 */
static void display_usage()
{
	printf( "sp-rtrace-postproc post-processor is used for resource trace data\n"
			"post processing. This includes freed allocation removal, backtrace\n"
			"compression etc. The post-processor accepts binary or text format\n"
			"data and outputs text format data. This allows it to use its own\n"
			"output as input data.\n"
			"Usage: sp-rtrace-postproc [<options>]\n"
			"where <options> are:\n"
			"  -i <path>        - the input file path. Standard input used by default.\n"
			"  -o <path>        - the output directory. Standard output is used if\n"
			"                     not set.\n"
			"  -t               - convert to the text format (optional, the output is always\n"
			"                     written in text format).\n"
			"  -l               - filter out matching allocs & frees i.e. list only 'leaks'.\n"
			"  -c               - compress trace by joining identical backtraces.\n"
			"  -r               - resolve function addresses in backtraces.\n"
			"  -C <mask>        - filter by context id <mask>.\n"
			"  -R <mask>        - filter by resource type <mask>.\n"
			"  -s <order>       - sort leaks by the specified order -\n"
			"                     size, size-asc, count, count-asc.\n"
			"  -b <depth>       - set maximum backtrace depth.\n"
			"  --include <file> - specify events to include in report.\n"
			"  --exclude <file> - specify events to exclude from report.\n"
			"                     For include/exclude options the events are stored in a\n"
			"                     text file, each line containing event index.\n"
			"  -q               - hide warning messages.\n"
			"  -h               - this help page.\n"
	);
}


static FILE* start_resolver(char* filename)
{
	int fds[2];
	if (pipe(fds) != 0) {
		msg_error("failed to create resolver pipe\n");
		exit (-1);
	}

	postproc_options.pid_resolve = fork();
	if (postproc_options.pid_resolve == -1) {
		msg_error("failed to fork resolver process\n");
		exit (-1);
	}
	if (postproc_options.pid_resolve == 0) {
		char* const args[] = {SP_RTRACE_RESOLVER, "-telf", filename ? "-o" : NULL, filename, NULL};
		close(fds[1]);
		dup2(fds[0], STDIN_FILENO);

		execv(SP_RTRACE_RESOLVER, args);
		msg_error("failed to execute resolver process %s (%s)\n", SP_RTRACE_RESOLVER, strerror(errno));
	}
	close(fds[0]);
	return fdopen(fds[1], "w");
}

/**
 * Writes resource trace data into text log.
 *
 * @param[in] rd   the resource trace data.
 * @return
 */
static void write_rtrace_log(rd_t* rd)
{
	FILE* fp;
	char path[PATH_MAX];
	char *output_file = NULL;

	/* If output directory is specified, create a file there.
	 * Otherwise write to standard output */
	if (postproc_options.output_dir) {
		if (get_log_filename(rd->pinfo->pid, postproc_options.output_dir, SP_RTRACE_TEXT_FILE_PATTERN, path, sizeof(path)) != 0) {
			msg_error("failed to make new log file name for directory %s\n", postproc_options.output_dir);
			exit (-1);
		}
		output_file = path;
	}
	if (postproc_options.resolve) {
		fp = start_resolver(output_file);
		if (fp == NULL) {
			msg_error("failed to get resolver pipe\n");
			exit (-1);
		}
	}
	else {
		if (output_file) {
			fp = fopen(output_file, "w");
			if (fp == NULL) {
				msg_error("failed to create log file %s\n", output_file);
				exit (-1);
			}
			printf("INFO: Created text log file %s\n", output_file);
		}
		else {
			fp = stdout;
		}
	}

	/* initialize formatted data wrapper structure */
	fmt_data_t fmt = {
			.fp = fp,
			.rd = rd,
			.comment = dlist_first(&rd->comments),
	};

	/* write the generic data - header, memory mapping, context registry */
	write_trace_environment(&fmt);

	/* write the function call trace data */
	write_trace_calls(&fmt);

	/* write the leak summary. Has sense only if the --remove-freed option is used */
	if (postproc_options.filter_leaks) {
		write_leak_summary(&fmt);
	}
	/* */
	if (postproc_options.output_dir || postproc_options.resolve) {
		fclose(fp);
	}
	if (postproc_options.pid_resolve) {
		int status;
		waitpid(postproc_options.pid_resolve, &status, 0);
	}
}

/**
 * Block the sigint from aborting the program execution.
 *
 * @param sig
 */
static void sigint_handler(int sig __attribute((unused)))
{
	postproc_abort = 1;
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[])
{
	tzset();

	/* install interrupt handler */
	struct sigaction sa = {.sa_flags = 0, .sa_handler = sigint_handler};
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		msg_error("failed to install SIGINT handler\n");
		return -1;
	}

	/* command line options */
	struct option long_options[] = {
			 {"input-file", 1, 0, 'i'},
			 {"output-dir", 1, 0, 'o'},
			 {"filter-leaks", 0, 0, 'l'},
			 {"compress", 0, 0, 'c'},
			 {"sort", 1, 0, 's'},
			 {"remove-args", 0, 0, 'a'},
			 {"resolve", 0, 0, 'r'},
			 {"context", 1, 0, 'C'},
			 {"resource", 1, 0, 'R'},
			 {"text", 0, 0, 't'},
			 {"help", 0, 0, 'h'},
			 {"backtrace-depth", 1, 0, 'b'},
			 {"include", 1, 0, 'I'},
			 {"exclude", 1, 0, 'X'},
			 {"quiet", 0, 0, 'q'},
			 {0, 0, 0, 0}
	};
	/* parse command line options */
	int opt;
	opterr = 0;
	
	while ( (opt = getopt_long(argc, argv, "i:o:tcs:ahrlC:R:b:q", long_options, NULL)) != -1) {
		switch(opt) {
		case 'h':
			display_usage();
			exit (0);

		case 'i':
			if (postproc_options.input_file) {
				msg_warning("overriding previously given option: -i %s\n", postproc_options.input_file);
				free(postproc_options.input_file);
			}
			postproc_options.input_file = strdup_a(optarg);
			break;

		case 'o':
			if (strcmp(optarg, "stdout")) {
				if (postproc_options.output_dir) {
					msg_warning("overriding previously given option: -o %s\n", postproc_options.output_dir);
					free(postproc_options.output_dir);
				}
				postproc_options.output_dir = strdup_a(optarg);
			}
			break;

		case 'l':
			postproc_options.filter_leaks = true;
			break;

		case 'c':
			postproc_options.compress = true;
			break;

		case 'r':
			postproc_options.resolve = true;
			break;

		case 'b':
			if (postproc_options.backtrace_depth != -1) {
				msg_warning("overriding previously given option: -b %d\n", postproc_options.backtrace_depth);
			}
			postproc_options.backtrace_depth = atoi(optarg);
			break;
			
		case 's':
			if (postproc_options.compare_leaks) {
				msg_warning("overriding previously given sort option (-s <order>)\n");
			}
			if (!strcmp(optarg, "size"))
				postproc_options.compare_leaks = (op_binary_t)leaks_compare_by_size_desc;
			else if (!strcmp(optarg, "size-asc"))
				postproc_options.compare_leaks = (op_binary_t)leaks_compare_by_size_asc;
			else if (!strcmp(optarg, "count"))
				postproc_options.compare_leaks = (op_binary_t)leaks_compare_by_count_desc;
			else if (!strcmp(optarg, "count-asc"))
				postproc_options.compare_leaks = (op_binary_t)leaks_compare_by_count_asc;
			break;

		case 'a':
			postproc_options.remove_args = true;
			break;

		case 'C':
			if (postproc_options.filter_context != -1) {
				msg_warning("overriding previously given option: -C %x\n", postproc_options.filter_context);
			}
			if (sscanf(optarg, "%x", &postproc_options.filter_context) != 1) {
				msg_error("invalid context mask: %s\n", optarg);
				exit (-1);
			}
			break;

		case 'R':
			if (postproc_options.filter_resource) {
				msg_warning("overriding previously given option: -R %x\n", postproc_options.filter_resource);
			}
			if (sscanf(optarg, "%x", &postproc_options.filter_resource) != 1) {
				msg_error("invalid resource type mask: %s\n", optarg);
				exit (-1);
			}
			break;
			
		case 'I':
			if (postproc_options.exclude_file) {
				msg_warning("include option overrides already specified exclude option\n");
				free(postproc_options.exclude_file);
				postproc_options.exclude_file = NULL;
			}
			postproc_options.include_file = strdup_a(optarg);
			break;

		case 'X':
			if (postproc_options.include_file) {
				msg_warning("exclude option is ignored beacuse of already specified include option\n");
			}
			else {
				postproc_options.exclude_file = strdup_a(optarg);
			}
			break;

		case 'q':
			msg_set_verbosity(MSG_ERROR);
			break;

		case '?':
			msg_error("unknown sp-rtrace-postproc option: %c\n", optopt);
			display_usage();
			exit (-1);
		
		case 't':
			break;
		}
	}
	if (optind < argc) {
		msg_error("unknown sp-rtrace-postproc argument: %s\n", argv[optind]);
		display_usage();
		exit(-1);
	}
	rd_t* rd = rd_create();
	if (rd == NULL) {
		msg_error("failed to create rtrace data container\n");
		exit (-1);
	}

	/* validate options and set default values */
	if (postproc_options.compress) {
		if (!postproc_options.compare_leaks)
			postproc_options.compare_leaks = (op_binary_t)leaks_compare_by_size_asc;
	}
	else if (postproc_options.compare_leaks) {
		msg_error("--sort option should be used with --filter-leaks and "
					"--compress options.\n");
		exit (-1);
	}

	/* determine the type of input stream - binary or text */
	int fd = STDIN_FILENO;
	/* read data from the specified file or standard input if no
	 * input file was specified with --input-file option */
	if (postproc_options.input_file) {
		fd = open(postproc_options.input_file, O_RDONLY);
		if (fd == -1) {
			msg_error("failed to open input file %s (%s)\n",
					postproc_options.input_file, strerror(errno));
			exit (-1);
		}
	}
	unsigned char proto_id;
	if (read(fd, &proto_id, 1) != 1) {
		msg_error("failed to read identification byte from the input stream.\n");
		exit (-1);
	}
	if (proto_id == SP_RTRACE_PROTO_HS_ID) {
		process_binary_data(rd, fd);
	}
	else {
		FILE* fp = fdopen(fd, "r");
		if (!fp) {
			msg_error("failed to reopen input stream.\n");
			exit (-1);
		}
		ungetc(proto_id, fp);
		process_text_data(rd, fp);
	}

	if (!rd->pinfo) {
		msg_error("failed to parse log header.\n");
		exit (-1);
	}

	if (postproc_options.backtrace_depth != -1) {
		filter_trim_backtraces(rd);
	}

	/* apply selected post-processing options */
	if (postproc_options.filter_resource) {
		filter_resource(rd);
	}

	if (postproc_options.filter_leaks) {
		filter_leaks(rd);
	}

	if (postproc_options.include_file) {
		filter_include(rd);
	}

	if (postproc_options.exclude_file) {
		filter_exclude(rd);
	}

	if (postproc_options.filter_context != -1) {
		filter_context(rd);
	}
	if (rd->hinfo) {
		filter_find_lowhigh_blocks(rd);
	}
	filter_update_resource_visibility(rd);

	/* write resulting output */
	write_rtrace_log(rd);

	rd_free(rd);

	free_options();

	return 0;
}

