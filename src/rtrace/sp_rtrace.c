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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
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
#include <dirent.h>
#include <dlfcn.h>
#include <ctype.h>

#include "sp_rtrace.h"
#include "rtrace_env.h"
#include "listener.h"

#include "rtrace_common.h"

#include "common/sp_rtrace_proto.h"
#include "common/utils.h"
#include "common/rtrace_data.h"
#include "debug_log.h"

/* Application exit condition, set by SIGINT */
sig_atomic_t rtrace_main_loop = 1;

/* rrace working mode, set by command options */
enum {
	MODE_UNDEFINED,
	MODE_EXECUTE,  /* start new process tracing */
	MODE_TOGGLE,   /* toggle tracing for existing process */
	MODE_LISTEN,   /* listen for data from main module */
};



/* The pre-processor options */
rtrace_options_t rtrace_options = {
		.output_dir = NULL,
		.manage_preproc = false,
		.preload = NULL,
		.audit = NULL,
		.start = false,
		.follow_forks = false,
		.backtrace_depth = NULL,
		.disable_timestamps = false,
		.enable_arguments = false,
		.postproc = NULL,
		.toggle_signal = NULL,
		.disable_packet_buffering = false,
		.pid = 0,
		.mode = MODE_UNDEFINED,
		.pid_postproc= 0,
};

/**
 * Display the help information.
 */
static void display_usage()
{
	printf( "sp-rtrace pre-processor can be used in two modes - to start a new process\n"
			"or to toggle tracing (enable/disable) for already running process.\n"
			"\n"
		    "1. Application tracing usage:\n"
		    "  sp-rtrace [<options>] -x <application> [<arg1> [<arg2>...]]]\n"
		    "  Executes and starts tracing <application> process with\n"
			"  arguments <arg1>, <arg2>...\n"
		    "  Where <options> are:\n"
			"  -o <outputdir>  - the directory for output files\n"
			"  -m              - instructs tracing module to start its\n"
			"                    own pre-processor process for data handling\n"
			"  -p <modules>    - a list of LD_PRELOAD tracing modules\n"
			"                    separated by ':'\n"
			"  -a <modules>    - a list of LD_AUDIT tracing modules\n"
			"                    separated by ':'\n"
			"  -s              - enable tracing from the start\n"
			"  -b <depth>      - the maximum number of function addresses\n"
			"                    in stack trace\n"
			"  -T              - disables timestamps in FC packets\n"
			"  -A              - enables function argument (FA) packets\n"
			"  -B              - disables packet buffering\n"
			"  -P <options>    - post-processor options. If this option\n"
			"                    is set the pre-processor will automatically\n"
			"                    launch the post-processor for data post-\n"
			"                    processing\n"
			"  Note that options should be always before the execute (-x)\n"
			"  switch.\n"
			"\n"
			"2. Tracing toggle usage:\n"
			"  sp-rtrace [-m] [-o <outputdir>] [-f] -t <pid>\n"
			"  Enables/disables tracing for the process <pid>.\n"
			"  Where:\n"
			"  -o <outputdir>  - the directory for output files\n"
			"  -m              - the target process was started in managed\n"
			"                    mode\n"
			"  -f              - send the toggle signal to all subprocesses\n"
			"                    recursively (not implemented)\n"
			"  -t <pid>        - pid of the process to toggle tracing for.\n"
			"\n"
			"3. Common options:\n"
			"  -S <signal>     - tracing toggle signal\n"
			"  -h              - this help page.\n"
			"  -l              - lists available tracing modules.\n"
			);
}

/**
 * Stops data processing and exits application.
 * @param sig
 */
static void sigint_handler(int sig)
{
	rtrace_main_loop = 0;
}

/**
 * Stops data processing and exits application if either traced process
 * or post-processor was terminated.
 * @param sig
 */
static void sigchld_handler(int sig)
{
	rtrace_main_loop = 0;
}

/**
 * Updates environment variables according to the specified command line arguments.
 *
 * @return
 */
static void set_environment()
{
	if (rtrace_options.output_dir) setenv(rtrace_env_opt[OPT_OUTPUT_DIR], rtrace_options.output_dir, 1);
	if (rtrace_options.manage_preproc) setenv(rtrace_env_opt[OPT_MANAGE_PREPROC], OPT_ENABLE, 1);
	if (rtrace_options.backtrace_depth) setenv(rtrace_env_opt[OPT_BACKTRACE_DEPTH], rtrace_options.backtrace_depth, 1);
	if (rtrace_options.disable_timestamps) setenv(rtrace_env_opt[OPT_DISABLE_TIMESTAMPS], OPT_ENABLE, 1);
	if (rtrace_options.enable_arguments) setenv(rtrace_env_opt[OPT_ENABLE_ARGUMENTS], OPT_ENABLE, 1);
	if (rtrace_options.postproc) setenv(rtrace_env_opt[OPT_POSTPROC], rtrace_options.postproc, 1);
	if (rtrace_options.toggle_signal) setenv(rtrace_env_opt[OPT_TOGGLE_SIGNAL], rtrace_options.toggle_signal, 1);
	if (rtrace_options.disable_packet_buffering) setenv(rtrace_env_opt[OPT_DISABLE_PACKET_BUFFERING], OPT_ENABLE, 1);
	if (rtrace_options.start) setenv(rtrace_env_opt[OPT_START], OPT_ENABLE, 1);

	if (rtrace_options.audit) setenv("LD_AUDIT", rtrace_options.audit, 1);

	char preload[PATH_MAX], *ppreload = preload;
	ppreload += sprintf(preload, SP_RTRACE_LIB_PATH "/%s:", SP_RTRACE_MAIN_MODULE);

	if (rtrace_options.preload) {
		char* module = strtok(rtrace_options.preload, ":");
		while (module) {
			int len = strlen(module);
			if (!strcmp(module + len - 3, ".so")) {
				ppreload += sprintf(ppreload, "%s:", module);
			}
			else {
				ppreload += sprintf(ppreload, SP_RTRACE_LIB_PATH "/libsp-rtrace-%s.so:", module);
			}
			module = strtok(NULL, ":");
		}
	}
	if (query_scratchbox()) {
		LOG("scratchbox environment detected");
		FILE* fp = fopen("/etc/ld.so.preload", "w");
		if (fp) {
			fputs(preload, fp);
			fclose(fp);
		}
		else {
			fprintf(stderr, "ERROR: failed to setup scratchbox preloading file /etc/ld.so.preload\n");
			exit (-1);
		}
	}
	else {
		setenv("LD_PRELOAD", preload, 1);
	}
}

/**
 * Releases resources allocated during option parsing.
 *
 * @return
 */
static void free_options()
{
	if (rtrace_options.output_dir) free(rtrace_options.output_dir);
	if (rtrace_options.preload) free(rtrace_options.preload);
	if (rtrace_options.audit) free(rtrace_options.audit);
	if (rtrace_options.backtrace_depth) free(rtrace_options.backtrace_depth);
	if (rtrace_options.postproc) free(rtrace_options.postproc);
	if (rtrace_options.toggle_signal) free(rtrace_options.toggle_signal);
}

/**
 * Opens pipe to post-processor.
 *
 * This function starts the post-processor and returns
 * the opened pipe to it.
 * @return  the pipe descriptor.
 */
static int open_postproc_pipe()
{
	int fd[2];
	if (pipe(fd) == -1) {
		fprintf(stderr, "ERROR: Failed to create pipe for post-processor\n");
		exit (-1);
	}
	rtrace_options.pid_postproc = fork();
	if (rtrace_options.pid_postproc == -1) {
		fprintf(stderr, "ERROR: Failed to fork post-processor process\n");
		exit (-1);
	}
	if (rtrace_options.pid_postproc == 0) {
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);

		/* create post-process argument list */
		char* argv[32], *ptr = rtrace_options.postproc;
		int argc = 0;
		char output_dir[512];

		argv[argc++] = SP_RTRACE_POSTPROC;
		/* forward --output-dir option to post-processor */
		if (rtrace_options.output_dir) {
			snprintf(output_dir, sizeof(output_dir), "--output-dir=%s", rtrace_options.output_dir);
			argv[argc++] = output_dir;
		}
		/* break argument string into separate arguments */
		while (argc < sizeof(argv) / sizeof(argv[0]) - 1) {
			argv[argc++] = ptr;
			ptr = strchr(ptr, ' ');
			if (!ptr) break;
			*ptr++ = '\0';
			while (*ptr == ' ') ptr++;
		}
		argv[argc] = NULL;

		execvp(SP_RTRACE_POSTPROC, argv);
		fprintf(stderr, "ERROR: failed to execute post-processor process %s (%s)\n",
				SP_RTRACE_POSTPROC, strerror(errno));
		exit (-1);
	}
	close(fd[0]);
	return fd[1];
}

/**
 * Create and open new log file.
 *
 * @return   the log file descriptor.
 */
static int open_output_file()
{
	char path[PATH_MAX];
	char* default_dir = ".";
	char* dir;

	dir = rtrace_options.output_dir;
	/* if (dir == NULL) dir = getenv("HOME"); */
	if (dir == NULL) dir = default_dir;

	if (get_log_filename(rtrace_options.pid, dir, SP_RTRACE_BINARY_FILE_PATTERN,
			SP_RTRACE_BINARY_FILE_EXT, path, sizeof(path)) != 0) {
		fprintf(stderr, "ERROR: failed to make new log file name for directory %s\n", dir);
		exit (-1);
	}
	printf("INFO: Opening binary log file %s\n", path);
	int fd =  open(path, O_CREAT | O_WRONLY | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == -1) {
		fprintf(stderr, "ERROR: failed to create log file %s (%s)\n", path, strerror(errno));
		exit (-1);
	}
	return fd;
}

/**
 * Creates pre-processor pipe for the target process.
 *
 * The path of the new pipe is returned in pipe_path.
 * @param[out] pipe_path   the output buffer.
 * @param[in] size         the output buffer size.
 * @return
 */
static void create_preproc_pipe(char* pipe_path, size_t size)
{
	snprintf(pipe_path, size, SP_RTRACE_PIPE_PATTERN, rtrace_options.pid);
	if (mkfifo(pipe_path, 0666) != 0) {
		fprintf(stderr, "ERROR: Failed to create named pipe %s\n", pipe_path);
		exit (-1);
	}
}

void rtrace_connect_output()
{
	if (rtrace_options.postproc) {
		/* post-processor options detected. Spawn sp-rtrace-postproc process
		 * for the sp-rtrace.
		 */
		fd_out = open_postproc_pipe();
	}
	else {
		/* Create and open binary log file */
		fd_out = open_output_file();
	}
}

/**
 * Disconnects the output data connection.
 *
 * @return
 */
static void disconnect_output()
{
	close (fd_out);
	if (rtrace_options.pid_postproc) {
		int status;
		waitpid(rtrace_options.pid_postproc, &status, 0);
	}
}

/**
 * Connects input descriptor either to named pipe or
 * standard input
 *
 * @param[in] pid   the target process pid (0 in managed mode).
 * @return
 */
static void connect_input(const char* pipe_path)
{
	if (pipe_path) {
		LOG("connecting to input pipe %s", pipe_path);
		fd_in = open(pipe_path, O_RDONLY);
	}
	else {
		fd_in = STDIN_FILENO;
	}
}

/**
 * Disconnects the input data connection.
 *
 * @param[in] pid   the target process pid (0 in managed mode).
 * @return
 */
static void disconnect_input(const char* pipe_path)
{
	if (pipe_path) {
		close(fd_in);
		remove(pipe_path);
	}
}

/**
 * Stop tracing the target process.
 *
 * The tracing is stopped simply by sending toggle signal to the
 * target process.
 * TODO: --follow-forks option support.
 * @param[in] signum   the toggle tracing signal number.
 * @return
 */
static void stop_tracing(int signum)
{
	LOG("sending toggle signal to process %d", rtrace_options.pid);
	kill(rtrace_options.pid, signum);
}


/**
 * Starts tracing the target process.
 *
 * @param[in] signum   the toggle tracing signal number.
 * @return
 */
static void begin_tracing(int signum)
{
	fprintf(stderr, "INFO: Tracing started. Trace output will be produced after "
		   "tracing is stopped. To stop tracing either use toggle option "
		   "again or terminate the target process.\n");
	if (rtrace_options.manage_preproc) {
		/* The pre-processor is managed by the tracing module.
		 * Simply send toggle signal and exit.
		 */
		kill(rtrace_options.pid, signum);
	}
	else {
		char pipe_path[128];
		/* setup data flow */

		/* the output stream will be setup after output settings
		 * (OS) packet is received.
		 * connect_output();
		 */
		create_preproc_pipe(pipe_path, sizeof(pipe_path));
		kill(rtrace_options.pid, signum);
		connect_input(pipe_path);

		/* start data processing */
		process_data();

		/* close data connection */
		disconnect_output();
		disconnect_input(pipe_path);

		/* remove pre-processor pipe */
		remove(pipe_path);
	}
}

/**
 * Toggles tracing for process rtrace_options.pid.
 *
 * @return             0 - success.
 */
static void toggle_tracing()
{
	char pipe_path[128];
	snprintf(pipe_path, sizeof(pipe_path), SP_RTRACE_PIPE_PATTERN, rtrace_options.pid);

	int signum = 0;
	if (rtrace_options.toggle_signal) signum = atoi(rtrace_options.toggle_signal);
	if (signum == 0) signum = SIGUSR1;

	if (access(pipe_path, F_OK) == 0) {
		stop_tracing(signum);
	}
	else {
		begin_tracing(signum);
	}
}


/**
 * Starts tracing for unmanaged mode.
 *
 * In unmanaged mode tracing module tries to connect pre-processor
 * named pipe, which means the pipe must be created before application
 * is executed.
 * @return    0 - success.
 */
static void start_process(char* app, char* args[])
{
	rtrace_options.pid = fork();
	if (rtrace_options.pid == -1) {
		fprintf(stderr, "ERROR: failed to fork target process\n");
		exit (-1);
	}
	if (rtrace_options.pid == 0) {
		if (rtrace_options.start) {
			char pipe_path[128];
			snprintf(pipe_path, sizeof(pipe_path), SP_RTRACE_PIPE_PATTERN, getpid());
			int spin = 0;
			while (access(pipe_path, W_OK)) {
				usleep(100000);
				if (spin++ > 50) {
					fprintf(stderr, "ERROR: A timeout occurred while waiting for the pre-processor pipe to be created");
					exit (-1);
				}
			}
		}
		/* either pipe has been created by the main process or tracing is not enabled
		 * at start. Set environment and execute app */
		set_environment();
		execvp(app, args);
		fprintf(stderr, "ERROR: failed to start process %s (%s)\n", app, strerror(errno));
		exit (-1);
	}
	if (rtrace_options.start) {
		char pipe_path[128];
		create_preproc_pipe(pipe_path, sizeof(pipe_path));
		connect_input(pipe_path);

		/* the output stream will be setup after output settings
		 * (OS) packet is received.
		 * connect_output();
		 */

		process_data();

		disconnect_output();
		disconnect_input(pipe_path);
	}
}


/**
 * Start tracing for managed mode.
 *
 * In managed mode tracing module spawns its own sp-rtrace process
 * for data pre-preprocessing. Just set the environment and spawn
 * target process.
 * @param[in] app     the application to start.
 * @param[in] args    the application arguments.
 * @return
 */
static void start_process_managed(char* app, char* args[])
{
	int pid = fork();
	if (pid == 0) {
		set_environment();
		execvp(app, args);
		fprintf(stderr, "ERROR: failed to start process %s (%s)\n", app, strerror(errno));
		exit (-1);
	}
}


/**
 * Managed mode data processing.
 *
 * In managed mode pre-processor is spawned
 * by the main tracing module itself to
 * handle its event data.
 * @return
 */
static void enter_listen_mode()
{
	/* the output stream will be setup after output settings
	 * (OS) packet is received.
	 * connect_output();
	 */
	connect_input(0);

	process_data();

	disconnect_input(0);
	disconnect_output();
}

/**
 * Prints module description.
 *
 * This function prints the description indented from the left
 * size and split into length character strings.
 * @param[in] indent       the indentation length.
 * @param[in] length       the maximum substring length.
 * @param[in] description  the description.
 */
void print_description(int indent, int length, const char* description)
{
	char buffer[80];
	char padding[80];
	memset(padding, ' ', indent);
	padding[indent] = '\0';
	const char* plimit = description + strlen(description);
	const char* pstart = description;

	while (true) {
		const char* pend = pstart + length;
		if (pend < plimit) {
			while (isalnum(*pend)) {
				if (pend == pstart) {
					fprintf(stderr, "WARNING: couldn't split the description (%s) into %d "
							        "character chunks\n", description, length);
					return;
				}
				pend--;
			}
			int len = pend - pstart + 1;
			memcpy(buffer, pstart, len);
			buffer[len] = '\0';
			pstart = pend + 1;
			while(*pstart == ' ') pstart++;
			printf("%s\n", buffer);
			printf("%s", padding);
		}
		else {
			printf("%s\n", pstart);
			break;
		}
	}
}

/**
 * Prints module information.
 *
 * This function attempts to load the specified module and
 * retrieve its module information with sp_rtrace_get_module_info
 * function. If module does not export this function, nothing is
 * printed.
 * @param[in] name  the module name.
 */
void print_module_info(const char* name) {
	char path[PATH_MAX];
	char types[] = {'?', 'P', 'A'};

	sprintf(path, SP_RTRACE_LIB_PATH "%s", name);
	void* lib = dlopen(path, RTLD_LAZY);
	if (lib == NULL) {
        fprintf (stderr, "%s\n", dlerror());
		return;
	}
	sp_rtrace_get_module_info_t get_module_info;
	get_module_info = (sp_rtrace_get_module_info_t)dlsym(lib, "sp_rtrace_get_module_info");
	if (get_module_info) {
		sp_rtrace_module_info_t* mi = get_module_info();
		printf("%c %-15s %2d.%-2d ", types[mi->type], mi->name, mi->version_major, mi->version_minor);
		print_description(24, 55, mi->description);
	}
	dlclose(lib);
}

/**
 * Lists rtrace modules located in /usr/lib/sp-rtrace directory.
 */
void list_modules()
{
	DIR* libdir = opendir("/usr/lib/" SP_RTRACE_LIB_DIR);
	if (libdir == NULL) {
		fprintf(stderr, "ERROR: failed to open module directory /usr/lib/" SP_RTRACE_LIB_DIR "\n");
		return;
	}
	/* preload the main module, as it exports symbols for submodules */
	void* mainlib = dlopen(SP_RTRACE_LIB_PATH SP_RTRACE_MAIN_MODULE, RTLD_LAZY | RTLD_GLOBAL);
	if (mainlib == NULL) {
        fprintf (stderr, "ERROR: %s\n", dlerror());
        closedir(libdir);
		return;
	}
	struct dirent *de;

	printf("T      Name      Version                  Description\n");
	printf("- --------------- ----- -----------------------------------------------------\n");
	while ((de = readdir(libdir)) != NULL) {
		if (de->d_type == DT_REG) {
			print_module_info(de->d_name);
		}
	}
	dlclose(mainlib);
	closedir(libdir);
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[])
{
	/* install interrupt handler */
	struct sigaction sa = {.sa_flags = 0, .sa_handler = sigint_handler};
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		fprintf(stderr, "ERROR: Failed to install SIGINT handler\n");
		return -1;
	}

	/* SIGCHLD handler was installed to track post-processor termination,
	 * but tracing process termination was messing up with the detection
	 * and thus the SIGCHLD handling was removed.
	 * In theory pre-processor has pipe to either process (traced process
	 * and post-processor) which should be intact until the child process
	 * terminates. So broken pipe will indicate that the associated process
	 * has been terminated.
	 */
	/* install SIGCHLD handler */
	/*
	sa.sa_handler = sigchld_handler;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		fprintf(stderr, "ERROR: Failed to install SIGCHILD handler\n");
		return -1;
	}
	*/

	/* parse command line options */
	int opt = 0;

	while ( opt != 'x' && (opt = getopt_long(argc, argv, rtrace_short_opt, rtrace_long_opt, NULL)) != -1) {
		switch(opt) {
		case 'o':
			rtrace_options.output_dir = strdup_a(optarg);
			break;

		case 'm':
			rtrace_options.manage_preproc = true;
			break;

		case 'p':
			rtrace_options.preload = strdup_a(optarg);
			break;

		case 'a':
			rtrace_options.audit = strdup_a(optarg);
			break;

		case 's':
			rtrace_options.start = true;
			break;

		case 't':
			rtrace_options.pid = atoi(optarg);
			if (rtrace_options.pid == 0) {
				fprintf(stderr, "ERROR: invalid pid value: %s", optarg);
				exit (-1);
			}
			rtrace_options.mode = MODE_TOGGLE;
			break;

		case 'f':
			rtrace_options.follow_forks = true;
			break;

		case 'b':
			rtrace_options.backtrace_depth = strdup_a(optarg);
			break;

		case 'T':
			rtrace_options.disable_timestamps = true;
			break;

		case 'A':
			rtrace_options.enable_arguments = true;
			break;

		case 'P':
			rtrace_options.postproc = strdup_a(optarg);
			break;

		case 'S':
			rtrace_options.toggle_signal = strdup_a(optarg);
			break;

		case 'B':
			rtrace_options.disable_packet_buffering = true;
			break;

		case 'h':
			display_usage();
			exit (0);

		case 'x':
			rtrace_options.mode = MODE_EXECUTE;
			break;

		case 'l':
			list_modules();
			exit (0);

		case 'L':
			rtrace_options.mode = MODE_LISTEN;
			break;

		default:
			fprintf(stderr, "ERROR: Unknown option %x(%c)\n", opt, opt);
			display_usage();
			exit (-1);
		}
	}

	switch (rtrace_options.mode) {
		case MODE_TOGGLE: {
			LOG("Switching to tracing toggle mode");
			toggle_tracing();
			break;
		}
		case MODE_EXECUTE: {
			LOG("Switching to process launch mode");
			if (rtrace_options.manage_preproc) {
				start_process_managed(optarg, argv + optind);
			}
			else {
				start_process(optarg, argv + optind);
			}
			break;
		}
		case MODE_LISTEN: {
			if (!rtrace_options.manage_preproc) {
				fprintf(stderr, "ERROR: -L mode is for internal use only\n");
				exit (-1);
			}
			LOG("Switching to listen mode");
			rtrace_options.pid = getppid();
			enter_listen_mode();
			break;
		}
		default: {
			fprintf(stderr, "ERROR: Failed to determine work mode, not enough options specified\n");
			display_usage();
			exit (-1);
		}
	}
	free_options();

	return 0;
}