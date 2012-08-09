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
#include "common/debug_log.h"
#include "common/msg.h"

#define MESSAGE_SIGINT "INFO: Trace was stopped, please wait for data retrieval to be finished.\n"

/* SIGCHLD handler was installed to track post-processor termination,
 * but tracing process termination was messing up with the detection
 * and thus the SIGCHLD handling was removed.
 * In theory pre-processor has pipe to either process (traced process
 * and post-processor) which should be intact until the child process
 * terminates. So broken pipe will indicate that the associated process
 * has been terminated.
 */
#define ENABLE_SIGCHLD_HANDLER 0


/* Application exit condition, set by SIGINT */
sig_atomic_t rtrace_stop_requests = 0;

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
		.start = false,
		.follow_forks = false,
		.backtrace_depth = NULL,
		.disable_timestamps = false,
		.postproc = NULL,
		.toggle_signal = SIGUSR1,
		.toggle_signal_name = NULL,
		.disable_packet_buffering = false,
		.pid = 0,
		.mode = MODE_UNDEFINED,
		.pid_postproc = 0,
		.output_file = NULL,
		.libunwind = false,
		.backtrace_all = false,
		.monitor_size = NULL,
};

/**
 * Display the help information.
 */
static void display_usage(void)
{
	printf("\nsp-rtrace pre-processor can be used in two modes - to start a new process\n"
	       "or to toggle tracing (enable/disable) for an already running process.\n"
	       "\n"
	       "1. Application tracing usage:\n"
	       "    sp-rtrace [<options>] -x <application> [<arg1> [<arg2>...]]]\n"
	       "  Executes and starts tracing <application> process with arguments\n"
	       "    <arg1>, <arg2>...\n"
	       "  Where the <options> are:\n"
	       "  -o <outputdir>  - the directory for output files\n"
	       "  -m              - instruct tracing module to start its own pre-processor\n"
	       "                    processes for data handling, needed for sub-process tracing\n"
	       "  -e <modules>    - a list of LD_PRELOAD tracing modules, separated by ':'\n"
	       "  -s              - enable tracing immediately at start\n"
	       "  -b <depth>      - the maximum number of function addresses in stack trace\n"
	       "  -T              - disable timestamps in function call packets\n"
	       "  -B              - disable packet buffering\n"
	       "  -P <options>    - post-processor options.  If this option is set,\n"
	       "                    the pre-processor will automatically launch\n"
	       "                    the post-processor for data post-processing\n"
	       "  -A              - get backtraces also for resource freeing.  By default\n"
	       "                    only resource allocation backtraces are reported\n"
	       "  -u              - use libunwind instead of libc backtrace() function\n"
	       "                    for stack trace unwinding\n"
	       "  -M S1[,S2...]   - report backtraces only for allocations of specified\n"
	       "                    size(s) S1, S2...\n"
	       "  Note that options must be given before the execute (-x) switch!\n"
	       "\n"
	       "2. Tracing toggle usage:\n"
	       "    sp-rtrace [-m] [-o <outputdir>] [-f] -t <pid>\n"
	       "  Enable/disable tracing for the process with given <pid>.\n"
	       "  Where:\n"
	       "  -f              - send the toggle signal to all subprocesses recursively\n"
	       "  -t <pid>        - pid of the process to toggle tracing for\n"
	       "\n"
	       "3. Common options:\n"
	       "  -S <signal>     - tracing toggle signal\n"
	       "  -h              - this help page\n"
	       "  -l              - lists available tracing modules\n"
	       "  -q              - hide warning messages\n"
	       "\n"
	       "Usage examples:\n"
	       "  Start 'sample' process with memory allocation tracing, enabled immediately.\n"
	       "  The (binary) output data will be stored to the current directory:\n"
	       "    sp-rtrace -s -e memory -x sample\n\n"
	       "  Additionally invoke sp-rtrace-postproc with the -r (resolve addresses)\n"
	       "  option and output resulting (ASCII) trace data to the standard output:\n"
	       "    sp-rtrace -s -e memory -P '-r' -x sample\n\n"
	       "  Invoke sp-rtrace-postproc with options '-l -c' (filter unfreed allocs\n"
	       "  and merge backtraces, see sp-rtrace-postproc manual) and store\n"
	       "  the resulting (ASCII) trace file to the current directory:\n"
	       "    sp-rtrace -s -e memory -P '-l -c' -o $(pwd) -x sample\n\n"
	       "  Toggle tracing for an already running 'sample' process:\n"
	       "    sp-rtrace -t $(pidof sample)\n\n"
	       "  Lists all available tracing modules:\n"
	       "    sp-rtrace -l\n\n"
	      );
}


/**
 * Stops data processing and exits application.
 * @param sig
 */
static void sigint_handler(int sig __attribute((unused)))
{
	rtrace_stop_requests++;
	/* send toggle signal to tracing process if the trace is active */
	if (fd_in && rtrace_options.pid) {
		int rc __attribute__((unused)) =  write(STDOUT_FILENO, MESSAGE_SIGINT, sizeof(MESSAGE_SIGINT));
		kill(rtrace_options.pid, rtrace_options.toggle_signal);
	}
}

#if ENABLE_SIGCHLD_HANDLER
/**
 * Stops data processing and exits application if either traced process
 * or post-processor was terminated.
 * @param sig
 */
static void sigchld_handler(int sig __attribute((unused)))
{
	rtrace_stop_requests = REQUEST_STOP;
}
#endif

/**
 * Updates environment variables according to the specified command line arguments.
 *
 * @return
 */
static void set_environment(void)
{
	char path[PATH_MAX];
	setenv(SP_RTRACE_READY, OPT_ENABLE, 1);
	if (rtrace_options.output_dir) setenv(rtrace_env_opt[OPT_OUTPUT_DIR], rtrace_options.output_dir, 1);
	if (rtrace_options.manage_preproc) setenv(rtrace_env_opt[OPT_MANAGE_PREPROC], OPT_ENABLE, 1);
	if (rtrace_options.backtrace_depth) setenv(rtrace_env_opt[OPT_BACKTRACE_DEPTH], rtrace_options.backtrace_depth, 1);
	if (rtrace_options.disable_timestamps) setenv(rtrace_env_opt[OPT_DISABLE_TIMESTAMPS], OPT_ENABLE, 1);
	if (rtrace_options.postproc) setenv(rtrace_env_opt[OPT_POSTPROC], rtrace_options.postproc, 1);
	if (rtrace_options.toggle_signal_name) setenv(rtrace_env_opt[OPT_TOGGLE_SIGNAL], rtrace_options.toggle_signal_name, 1);
	if (rtrace_options.disable_packet_buffering) setenv(rtrace_env_opt[OPT_DISABLE_PACKET_BUFFERING], OPT_ENABLE, 1);
	if (rtrace_options.start) setenv(rtrace_env_opt[OPT_START], OPT_ENABLE, 1);
	if (rtrace_options.backtrace_all) setenv(rtrace_env_opt[OPT_BACKTRACE_ALL], OPT_ENABLE, 1);
	if (rtrace_options.libunwind) setenv(rtrace_env_opt[OPT_LIBUNWIND], OPT_ENABLE, 1);
	if (rtrace_options.monitor_size) setenv(rtrace_env_opt[OPT_MONITOR_SIZE], rtrace_options.monitor_size, 1);
	if (getcwd(path, sizeof(path))) {
		setenv(SP_RTRACE_START_DIR, path, 1);
		/* force current directory for output files if no output directory is specified */
		if (!rtrace_options.output_dir) setenv(rtrace_env_opt[OPT_OUTPUT_DIR], path, 1);
	}

	char preload[PATH_MAX], *ppreload = preload;
	ppreload += sprintf(preload, "%s:", SP_RTRACE_MAIN_MODULE);

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
			msg_error("failed to setup scratchbox preloading file /etc/ld.so.preload\n");
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
static void free_options(void)
{
	if (rtrace_options.output_dir) free(rtrace_options.output_dir);
	if (rtrace_options.preload) free(rtrace_options.preload);
	if (rtrace_options.backtrace_depth) free(rtrace_options.backtrace_depth);
	if (rtrace_options.postproc) free(rtrace_options.postproc);
	if (rtrace_options.toggle_signal_name) free(rtrace_options.toggle_signal_name);
	if (rtrace_options.output_file) free(rtrace_options.output_file);
	if (rtrace_options.monitor_size) free(rtrace_options.monitor_size);
}

/**
 * Opens pipe to post-processor.
 *
 * This function starts the post-processor and returns
 * the opened pipe to it.
 * @return  the pipe descriptor.
 */
static int open_postproc_pipe(void)
{
	int fd[2];
	if (pipe(fd) == -1) {
		msg_error("failed to create pipe for post-processor\n");
		exit (-1);
	}
	rtrace_options.pid_postproc = fork();
	if (rtrace_options.pid_postproc == -1) {
		msg_error("failed to fork post-processor process\n");
		exit (-1);
	}
	if (rtrace_options.pid_postproc == 0) {
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);

		/* create post-process argument list */
		char* argv[32];
		char argv0[] = SP_RTRACE_POSTPROC;
		char* ptr = rtrace_options.postproc;
		int argc = 0;
		char output_dir[512];
		
		argv[argc++] = argv0;
		/* forward --output-dir option to post-processor */
		if (rtrace_options.output_dir) {
			snprintf(output_dir, sizeof(output_dir), "--output-dir=%s", rtrace_options.output_dir);
			argv[argc++] = output_dir;
		}
		/* break argument string into separate arguments */
		if (*ptr) {
			while (argc < (int)sizeof(argv) / (int)sizeof(argv[0]) - 1) {
				argv[argc++] = ptr;
				ptr = strchr(ptr, ' ');
				if (!ptr) break;
				*ptr++ = '\0';
				while (*ptr == ' ') ptr++;
			}
		}
		argv[argc] = NULL;

		setpgrp();
		execv(SP_RTRACE_POSTPROC, argv);
		msg_error("failed to execute post-processor process %s (%s)\n",
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
static int open_output_file(void)
{
	char path[PATH_MAX];
	const char* default_dir = ".";
	const char* dir;

	dir = rtrace_options.output_dir;
	/* if (dir == NULL) dir = getenv("HOME"); */
	if (dir == NULL || !strcmp(dir, "stdout")) dir = default_dir;

	if (get_log_filename(rtrace_options.pid, dir, SP_RTRACE_BINARY_FILE_PATTERN, path, sizeof(path)) != 0) {
		msg_error("failed to make new log file name for directory %s\n", dir);
		return -1;
	}
	int fd =  open(path, O_CREAT | O_WRONLY | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == -1) {
		msg_error("failed to create log file %s (%s)\n", path, strerror(errno));
		return -1;
	}
	rtrace_options.output_file = strdup_a(path);
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
	snprintf(pipe_path, size, SP_RTRACE_PIPE_PATTERN "%d", rtrace_options.pid);
	if (access(pipe_path, F_OK) != 0 && mkfifo(pipe_path, 0666) != 0) {
		msg_error("failed to create named pipe %s\n", pipe_path);
		exit (-1);
	}
}

int rtrace_connect_output(void)
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
	return fd_out;
}

/**
 * Disconnects the output data connection.
 *
 * @return
 */
static void disconnect_output(void)
{
	if (fd_out) {
		close (fd_out);
		if (rtrace_options.pid_postproc) {
			int status;
			waitpid(rtrace_options.pid_postproc, &status, 0);
		}
		else {
			printf("INFO: Created binary log file %s\n", rtrace_options.output_file);
		}
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
		int rc = remove(pipe_path);
		if (rc == -1) {
			msg_error("failed to removing pipe: %s\n", strerror(errno));
		}
	}
}

/* ---------- trace toggling specific code ---------------- */

/**
 * Stop tracing the *non-managed* target process.
 *
 * The tracing is stopped simply by sending toggle signal to the
 * target process.
 * @return
 */
static void stop_tracing(void)
{
	fprintf(stderr, "INFO: Tracing stopped. The log file will be created shortly.\n");
	kill(rtrace_options.pid, rtrace_options.toggle_signal);
}

/**
 * Start tracing the *non-managed* target process.
 *
 * @return
 */
static void begin_tracing(void)
{
	char pipe_path[128];
	fprintf(stderr, "INFO: Tracing started. Trace output will be produced after "
	        "tracing is stopped. To stop tracing either press Ctrl+C, use toggle option "
	        "again or terminate the target process.\n");

	/* setup data flow */

	/* the output stream will be setup after output settings
	 * (OS) packet is received.
	 * connect_output();
	 */
	create_preproc_pipe(pipe_path, sizeof(pipe_path));
	kill(rtrace_options.pid, rtrace_options.toggle_signal);
	connect_input(pipe_path);

	/* start data processing */
	int rc  = process_data();
	/* close data connection */
	disconnect_output();
	disconnect_input(pipe_path);

	exit(rc);
}

/**
 * Toggle tracing of the *managed* target process.
 *
 * The tracing is toggled simply by sending toggle signal
 * to the target process.
 * @return
 */
static void toggle_managed_tracing(void)
{
	fprintf(stderr, "INFO: toggling tracing for the process %d started in managed mode.\n", rtrace_options.pid);
	kill(rtrace_options.pid, rtrace_options.toggle_signal);
}

/**
 * Checks if pid is a child process of ppid.
 * 
 * @param[in] pid   the child process to check.
 * @param[in] ppid  the parent process to check.
 * @return
 */
static bool is_child_process_of(int pid, int ppid)
{
	int rc = false;
	char buffer[1024];
	sprintf(buffer, "/proc/%d/status", pid);
	FILE* fp = fopen(buffer, "r");
	if (fp) {
		while (fgets(buffer, sizeof(buffer), fp)) {
			int tpid;
			if (sscanf(buffer, "PPid:\t%d", &tpid) == 1) {
				rc = (ppid == tpid);
				break;
			}
		}
		fclose(fp);
	}
	return rc;
}


/**
 * Spawn sp-rtrace to toggle tracing for the specified child process.
 * (This is a bit wasteful way to do this, but simplifies code)
 * 
 * @param[in] cpid   the child process id.
 */
static void toggle_child_process(int cpid)
{
	int pid = fork();
	if (pid) return;
	
	char spid[16];
	sprintf(spid, "%d", cpid);
		
	const char* args[10] = {SP_RTRACE_PREPROC, "--follow-forks", "-t", spid};
	const char** parg = args + 4;
	if (rtrace_options.toggle_signal_name) {
		*parg++ = "-S";
		*parg++ = rtrace_options.toggle_signal_name;
	}
	if (rtrace_options.manage_preproc) {
		*parg++ = "-m";
	}
	*parg++ = NULL;
	execv(SP_RTRACE_PREPROC, args);
}


/**
 * Toggles tracing for child processes.
 * 
 * This function iterates /proc/ filesystem and spawns sp-rtrace toggle
 * process for every child process.
 */
static void toggle_child_processes(int pid)
{
	DIR* dir = opendir("/proc");
	if (dir == NULL) {
		msg_error("failed to open /proc/ directory\n");
		return;
	}
	struct dirent *de;
	while ((de = readdir(dir)) != NULL) {
		if (de->d_type == DT_DIR) {
			int cpid = atoi(de->d_name);
			if (cpid && is_child_process_of(cpid, pid)) {
				toggle_child_process(cpid);
			}
		}
	}
	closedir(dir);
}


/**
 * Checks if the specified process tracing is in managed mode.
 * 
 * The check is done by checking whether process has the environment
 * variable controlling managed tracing.
 * @param[in] pid  the target process identifier.
 * @return    true if the process is managed, false otherwise.
 */
static bool is_process_managed(int pid)
{
	int fd;
	char buffer[PATH_MAX];
	sprintf(buffer, "/proc/%d/environ", pid);
	fd = open(buffer, O_RDONLY);
	if (fd >= 0) {
		int count, len = strlen(rtrace_env_opt[OPT_MANAGE_PREPROC]);
		char *start, *end;
		start = buffer;
		count = sizeof(buffer);
		while ((count = read(fd, start, count)) > 0) {
			/* contains: <name>=<value>\0 ?
			 * (value expected to be 0|1)
			 */
			start = buffer;
			while (count >= len+3) {
				if (strncmp(start, rtrace_env_opt[OPT_MANAGE_PREPROC], len) == 0) {
					start += len + 1;
					return atoi(start);
				}
				end = memchr(start, '\0', count);
				if (!end) {
					break;
				}
				end++;
				count -= (end - start);
				start = end;
			}
			memmove(buffer, start, count);
			start = buffer + count;
			count = sizeof(buffer) - count;
		}
		close(fd);
	}
	return false;
}

/**
 * Checks if the specified process is being traced.
 * 
 * The check is simply done by verifying if the process
 * has mapped the main tracing module.
 * @param[in] pid  the target process identifier.
 * @return    true if the process is being traced, false otherwise.
 */
static bool is_process_traced(int pid)
{
	bool rc = false;
	char buffer[PATH_MAX];
	sprintf(buffer, "/proc/%d/maps", pid);
	FILE* fp = fopen(buffer, "r");
	if (fp) {
		while (fgets(buffer, sizeof(buffer), fp)) {
			if (strstr(buffer, SP_RTRACE_MAIN_MODULE)) {
				rc = true;
				break;
			}
		}
		fclose(fp);
	}
	return rc;
}

/**
 * Checks if trace is running for the specified *non-managed* process.
 * @param[in] pid  the target process identifier.
 * @return    true if the process tracing is enabled
 */
static bool is_process_tracing_enabled(int pid)
{
	char pipe_path[128];
	/* First check if the named pipe for the process exist (normal mode check) */
	snprintf(pipe_path, sizeof(pipe_path), SP_RTRACE_PIPE_PATTERN "%d", pid);
	if (access(pipe_path, F_OK) == 0) return true;
	return false;
}

/**
 * Toggles tracing for process rtrace_options.pid.
 *
 * @return             0 - success.
 */
static void toggle_tracing(void)
{
	pid_t pid = rtrace_options.pid;
	/* first check if the target process is launched in tracing mode */
	if (!is_process_traced(pid)) {
		msg_error("process %d doesn't have sp-rtrace LD_PRELOAD module. Was it started with sp-rtrace tool?\n", rtrace_options.pid);
		return;
	}

	rtrace_options.manage_preproc = is_process_managed(pid);

	if (rtrace_options.follow_forks) {
		toggle_child_processes(pid);
	}

	if (rtrace_options.manage_preproc) {
		toggle_managed_tracing();
	} else {
		if (is_process_tracing_enabled(pid)) {
			stop_tracing();
		} else {
			begin_tracing();
		}
	}
}

/* -------- handling of other trace modes ------------- */

/**
 * Starts tracing for unmanaged mode.
 *
 * In unmanaged mode tracing module tries to connect pre-processor
 * named pipe, which means the pipe must be created before application
 * is executed.
 * @return    0 - success.
 */
static int start_process(char* app, char* args[])
{
	rtrace_options.pid = fork();
	if (rtrace_options.pid == -1) {
		msg_error("failed to fork target process\n");
		exit (-1);
	}
	if (rtrace_options.pid == 0) {
		if (rtrace_options.start) {
			char pipe_path[sizeof(SP_RTRACE_PIPE_PATTERN) + 16];
			snprintf(pipe_path, sizeof(pipe_path), SP_RTRACE_PIPE_PATTERN "%d", getpid());
			int spin = 0;
			while (access(pipe_path, W_OK)) {
				usleep(100000);
				if (spin++ > 50) {
					msg_error("a timeout occurred while waiting for the pre-processor pipe to be created");
					exit (-1);
				}
			}
		}
		/* either pipe has been created by the main process or tracing is not enabled
		 * at start. Set environment and execute app */
		setpgrp();
		set_environment();
		execvp(app, args);
		msg_error("failed to start process %s (%s)\n", app, strerror(errno));
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

		int rc = process_data();

		disconnect_output();
		disconnect_input(pipe_path);

		/* Finally if the rtrace was aborted by SIGINT, forward it to the
		 * target process after the trace has been switched off. */
		if (rtrace_stop_requests) {
			kill(rtrace_options.pid, SIGINT);
		}

		return rc;
	}
	return 0;
}


/**
 * Start tracing for managed mode.
 *
 * In managed mode tracing module spawns its own sp-rtrace process
 * for data pre-preprocessing. Just set the environment and exec
 * target process.
 * @param[in] app     the application to start.
 * @param[in] args    the application arguments.
 * @return
 */
static void start_process_managed(char* app, char* args[])
{
	set_environment();
	execvp(app, args);
	msg_error("failed to start process %s (%s)\n", app, strerror(errno));
	exit (-1);
}

/**
 * From things listed in "How do I get my program to act like a daemon?":
 *	http://www.faqs.org/faqs/unix-faq/programmer/faq/
 * (or Stevens, Advanced Programming in the Unix Environment)
 * 
 * This does just double fork (to prevent traced process from getting
 * SIGCHLD in non-managed mode), setsid() and change to root (to not
 * block current dir). Closing of stdout/err aren't done because messages
 * should still go to console.
 */
static void daemonize(void)
{
	const char* dir;
	int pid = fork();
	if (pid == -1) {
		msg_error("failed to fork in daemonize()\n");
		exit (-1);
	}
	if (pid > 0) {
		_exit(0);
	}
	if (setsid() < 0) {
		msg_error("setsid: %s\n", strerror(errno));
	}
	pid = fork();
	if (pid > 0) {
		_exit(0);
	}

	dir = rtrace_options.output_dir;
	if (dir && strcmp(dir, "stdout") != 0) {
		/* output dir set, go to root */
		chdir("/");
	}
}

/**
 * Managed mode data processing.
 *
 * In managed mode pre-processor is spawned
 * by the main tracing module or rtrace-start
 * to handle its event data.
 * @return
 */
static void enter_listen_mode(const char* pipe_path)
{
	LOG("Entering listen mode");

	daemonize();

	connect_input(pipe_path);

	int rc = process_data();

	disconnect_input(pipe_path);
	disconnect_output();
	exit (rc);
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
static void print_description(int indent, int length, const char* description)
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
					msg_warning("couldn't split the description (%s) into %d "
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
static void print_module_info(const char* name) {
	char path[PATH_MAX];
	char types[] = {'?', 'P', 'A'};

	sprintf(path, SP_RTRACE_LIB_PATH "/%s", name);
	void* lib = dlopen(path, RTLD_LAZY);
	if (lib == NULL) {
        msg_error("%s\n", dlerror());
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
 * Lists rtrace modules located in sp-rtrace library directory.
 */
static void list_modules(void)
{
	DIR* libdir = opendir(SP_RTRACE_LIB_PATH);
	if (libdir == NULL) {
		msg_error("failed to open module directory " SP_RTRACE_LIB_PATH "\n");
		return;
	}
	/* preload the main module, as it exports symbols for submodules */
	void* mainlib = dlopen(SP_RTRACE_MAIN_MODULE, RTLD_LAZY | RTLD_GLOBAL);
	if (mainlib == NULL) {
		msg_error("%s\n", dlerror());
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
 * Translates signal name (SIG???) into its value in string format.
 *
 * This function allocates string to store the result, which
 * should be freed by the caller afterwards.
 * Numeric names are treated as already translated signal values.
 * For example passing "SIGUSR2" allocates and returns string "12".
 * Passing "11" allocates and returns string "11".
 * If unrecognized name is specified, the default signal value "10"
 * (SIGUSR1) is returned.
 * @param name   the signal name.
 * @return       the signal value.
 */
static char* translate_signal(const char* name)
{
	if (atoi(name) > 0) return strdup_a(name);

	int signum = SIGUSR1;
	if (!strcmp(name, "SIGUSR2")) signum = SIGUSR2;

	char signame[32];
	snprintf(signame, sizeof(signame), "%d", signum);
	return strdup_a(signame);
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
		msg_error("Failed to install SIGINT handler\n");
		return -1;
	}

#if ENABLE_SIGCHLD_HANDLER
	/* install SIGCHLD handler */
	sa.sa_handler = sigchld_handler;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		msg_error("Failed to install SIGCHILD handler\n");
		return -1;
	}
#endif

	/* parse command line options */
	int opt = 0;
	opterr = 0;

	while ( opt != 'x' && (opt = getopt_long(argc, argv, rtrace_short_opt, rtrace_long_opt, NULL)) != -1) {
		switch(opt) {
		case 'o':
			if (rtrace_options.output_dir) {
				msg_warning("overriding previously given option: -o %s\n", rtrace_options.output_dir);
				free(rtrace_options.output_dir);
			}
			rtrace_options.output_dir = strdup_a(optarg);
			break;

		case 'm':
			rtrace_options.manage_preproc = true;
			break;

		case 'e':
			if (rtrace_options.preload) {
				msg_warning("overriding previously given option: -p %s\n", rtrace_options.preload);
				free(rtrace_options.preload);
			}
			rtrace_options.preload = strdup_a(optarg);
			break;

		case 's':
			rtrace_options.start = true;
			break;

		case 't':
			rtrace_options.pid = atoi(optarg);
			if (rtrace_options.pid == 0) {
				msg_error("invalid pid value: %s", optarg);
				exit (-1);
			}
			rtrace_options.mode = MODE_TOGGLE;
			break;

		case 'f':
			rtrace_options.follow_forks = true;
			break;

		case 'b':
			if (rtrace_options.backtrace_depth) {
				msg_warning("overriding previously given option: -b %s\n", rtrace_options.backtrace_depth);
				free(rtrace_options.backtrace_depth);
			}
			rtrace_options.backtrace_depth = strdup_a(optarg);
			break;

		case 'T':
			rtrace_options.disable_timestamps = true;
			break;

		case 'A':
			rtrace_options.backtrace_all = true;
			break;

		case 'P':
			if (rtrace_options.postproc) {
				msg_warning("overriding previously given option: -P %s\n", rtrace_options.postproc);
				free(rtrace_options.backtrace_depth);
			}
			rtrace_options.postproc = strdup_a(optarg ? optarg : "");
			break;

		case 'S': {
			rtrace_options.toggle_signal_name = translate_signal(optarg);
			int signum = atoi(rtrace_options.toggle_signal_name);
			if (signum > 0) rtrace_options.toggle_signal = signum;
			break;
		}
		case 'B':
			rtrace_options.disable_packet_buffering = true;
			break;
		
		case 'u':
			rtrace_options.libunwind = true;
			break;
			
		case 'M':
			rtrace_options.monitor_size = strdup_a(optarg);
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
			if (optarg) {
				rtrace_options.pid = atoi(optarg);
			}
			break;

		case 'q':
			msg_set_verbosity(MSG_ERROR);
			break;

		case '?':
			display_usage();
			msg_error("unknown sp-rtrace option: %c\n", optopt);
			exit (-1);
		}
	}
	int rc = 0;

	switch (rtrace_options.mode) {
		case MODE_TOGGLE: {
			LOG("Switching to tracing toggle mode");
			toggle_tracing();
			break;
		}
		case MODE_EXECUTE: {
			LOG("Switching to process launch mode");
			if (rtrace_options.manage_preproc) {
				start_process_managed(optarg, argv + optind - 1);
			}
			else {
				rc = start_process(optarg, argv + optind - 1);
			}
			break;
		}
		case MODE_LISTEN: {
			if (rtrace_options.pid) {
				/* Target process pid is specified if sp-trace was launched from trace
				 * script. In this case sp-trace should manage the named pipe */
				char pipe_path[128];
				create_preproc_pipe(pipe_path, sizeof(pipe_path));
				enter_listen_mode(pipe_path);
			} else {
				if (!rtrace_options.manage_preproc) {
					msg_error("-L mode is for internal use only\n");
					exit (-1);
				}
				rtrace_options.pid = getppid();
				enter_listen_mode(NULL);
			}
			break;
		}
		default: {
			display_usage();
			msg_error("failed to determine work mode, not enough options specified\n");
			exit (-1);
		}
	}
	free_options();

	return rc;
}
