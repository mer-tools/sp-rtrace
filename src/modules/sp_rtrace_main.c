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
#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <execinfo.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <malloc.h>

#include "rtrace/rtrace_env.h"
#include "rtrace_common.h"
#include "sp_rtrace_main.h"
#include "sp_context_impl.h"
#include "common/debug_log.h"
#include "common/sp_rtrace_proto.h"
#include "library/sp_rtrace_defs.h"
#include "common/utils.h"
#include "libunwind_support.h"

/* module information */
static sp_rtrace_module_info_t module_info = {
		.type = MODULE_TYPE_UNDEFINED,
		.version_major = 1,
		.version_minor = 0,
		.name = "main",
		.description = "Main tracing module providing pre-processor "
				       "communication and other tracing module management.",
};

/* */
#define DEFAULT_BACKTRACE_DEPTH    10

/* pre-processor pipe descriptor */
static int fd_proc = 0;

/*  pre-processor pipe path */
static char pipe_path[sizeof(SP_RTRACE_PIPE_PATTERN) + 16];

/* backtrace lock for thread synchronization */
__thread volatile sync_entity_t backtrace_lock = 0;

/* heap statistics */
static struct mallinfo heap_info;
static pointer_t heap_bottom = 0;

static fn_backtrace_t backtrace_impl = backtrace;

static char proc_name[PATH_MAX];

/**
 * The default options.
 */
static sp_rtrace_options_t rtrace_main_options = {
	.backtrace_depth = DEFAULT_BACKTRACE_DEPTH,
	.enable_timestamps = true,
	.enable_ext_args = false,
	.enable = false,
	.manage_preproc = false,
	.enable_packet_buffering = true,
	.output_dir = {0},
	.postproc = {0},
	.backtrace_all = false,
};

sp_rtrace_options_t* sp_rtrace_options = &rtrace_main_options;


/* unnecessary backtrace frames that can be stripped */
#ifdef __arm__
 #define BT_SKIP_TOP       4
#else
 #define BT_SKIP_TOP       3
#endif
#define BT_SKIP_BOTTOM     0


/*
 * Module registry
 */

/**
 * Submodule data.
 * Currently used only to disable tracing in submodules.
 * Each submodule must call sp_rtrace_register_module during its
 * initialization and pass its trace enable/disable function.
 */
typedef struct rtrace_module_t {
    const char* name;
    int id;
    unsigned char vmajor;
    unsigned char vminor;
	sp_rtrace_enable_tracing_t enable;
} rtrace_module_t;

/* trace submodules */
static rtrace_module_t rtrace_modules[16];
static unsigned int rtrace_module_index = 0;


/*
 * Resource type registry
 */
static sp_rtrace_resource_t rtrace_resources[32];
static unsigned int rtrace_resource_index = 0;

/**
 * Enables/disables tracing.
 * @param[in] value   true to enable tracing, false to disable.
 * @return
 */
static void enable_tracing(bool value)
{
	unsigned int i;
	for (i = 0; i < rtrace_module_index; i++) {
		rtrace_modules[i].enable(value);
	}
}


/**
 * Opens named pipe to the pre-processor.
 *
 * First this function checks if the necessary named pipe exists.
 * If not, it launches pre-processor (sp-rtrace-proc) and
 * waits until it creates the named pipe.
 * @return    the opened pipe descriptor.
 */
static int open_pipe()
{
	if (sp_rtrace_options->manage_preproc) {
		LOG("spawning pre-processor process");
		/* spawn the pre-processor process if */
		int fd[2];
		if (pipe(fd) == -1) return -1;
		pid_t pid = fork();
		if (pid == -1) return -1;
		if (pid == 0) {
			close(fd[1]);
			dup2(fd[0], STDIN_FILENO);

			rtrace_args_t args;
			rtrace_args_scan_env(&args, SP_RTRACE_PREPROC);
			rtrace_args_add_opt(&args, 'L', NULL);
			rtrace_args_finish(&args);

#ifdef DEBUG_INFO
			char** ptr = args.argv;
			while (*ptr) {
				LOG("\t'%s'", *ptr++);
			}
#endif
			execve(INSTALL_DIR "/bin/" SP_RTRACE_PREPROC, args.argv, NULL);
			fprintf(stderr, "ERROR: Failed to execute pre-processor process %s (%s)\n",
					SP_RTRACE_PREPROC, strerror(errno));
			exit (-1);
		}
		close(fd[0]);
		return fd[1];
	}
	else {
		LOG("connecting to output pipe %s", pipe_path);
		/* The pre-processor pipe should be already created.
		   Otherwise disable the tracing.
		 */
		if (access(pipe_path, W_OK)) {
			MSG_ERROR_CONST("WARNING: Failed to open pre-processor pipe. "
					  "This could be a cause if sp-rtrace was not started in managed mode (-m) "
					  "and toggle signal was not sent with sp-rtrace toggle (-t) option.\n"
					  "Tracing is not enabled.\n");
			sp_rtrace_options->enable = false;
			enable_tracing(false);
			return -1;
		}
		return open(pipe_path, O_WRONLY);
	}
}

/**
 * Closes the pre-processor pipe.
 *
 * This function is provided only for symmetry with open_pipe() function
 * and could be removed if deemed unnecessary.
 * @param[in] fd   the pre-processor pipe.
 * @return
 */
static void close_pipe(int fd)
{
	close(fd);

	int status;
	wait(&status);
}

/*
 * Local buffer for write caching implementation.
 */

/* The sending (default pipe) buffer size */
#define BUFFER_SIZE    4096

/* local pipe buffer (2x sending  buffer size) */
static char pipe_buffer[BUFFER_SIZE << 1];

/* local buffer head */
static char* pipe_buffer_head = pipe_buffer;

/* local buffer locking variable */
static sync_entity_t pipe_buffer_locked = 0;

/**
 * Writes pipe buffer into the pre-processor pipe.
 *
 * @return   the number of bytes written.
 */
static int pipe_buffer_flush()
{
	int size = pipe_buffer_head - pipe_buffer;
	if (write(fd_proc, pipe_buffer, size) < 0) {
		MSG_ERROR_CONST("ERROR: failed to write data into pipe, disabling tracing.\n");
		enable_tracing(false);
		sp_rtrace_options->enable = false;
		fd_proc = 0;
	}
	pipe_buffer_head = pipe_buffer;
	return size;
}


/**
 * Locks pipe buffer and returns pointer to the writable area in buffer.
 *
 * @return   a pointer to the writable area in pipe buffer.
 */
static char* pipe_buffer_lock()
{
	while ( !sync_bool_compare_and_swap(&pipe_buffer_locked, 0, 1));
	return pipe_buffer_head;

}
/**
 * Unlocks pipe buffer and flushes it if necessary (before unlocking).
 *
 * @param[in] ptr   the writable area pointer returned by pipe_buffer_lock().
 * @param[in] size  the number of bytes written to the are.
 * @return
 */
static void pipe_buffer_unlock(const char* ptr, int size)
{
	if (ptr != pipe_buffer_head) return;
	/* writes the data into pipe either if  the send buffer
	 * (which is half of the allocated pipe buffer) is full.
	 */
	if (ptr + size > pipe_buffer + BUFFER_SIZE) {
		pipe_buffer_flush();
		/* move the last packet to the beginning of pipe buffer */
		while (pipe_buffer_head  <  pipe_buffer + size) {
			*pipe_buffer_head++ = *ptr++;
		}
	}
	else {
		pipe_buffer_head += size;
		/* if the buffering is disabled flush buffer after every write */
		if (!sp_rtrace_options->enable_packet_buffering) {
			pipe_buffer_flush();
		}
	}
	pipe_buffer_locked = 0;
}

/**
 * Resets the pipe buffer
 *
 * @return
 */
static void pipe_buffer_reset()
{
	pipe_buffer_head = pipe_buffer;
}

/*
 *
 */

/**
 * Retrieve name of the current process.
 *
 * @param[out] out   the output buffer.
 * @param[in] size   the output buffer size.
 * @return
 */
static void get_proc_name(char* out, size_t size)
{
	int fd = open("/proc/self/cmdline", O_RDONLY);
	if (fd != -1) {
		int n = read(fd, out, size);
		if (n >= 0) {
			out[n] = '\0';
		}
		close(fd);
	}
}

/**
 * Writes module info packet into processor pipe.
 *
 * @param[in] id      the module id.
 * @param[in] name    the module name.
 * @param[in] major   the module version number (major).
 * @param[in] minor   the module version number (minor).
 * @return            the number of bytes written.
 */
static int write_module_info(int id, const char* name, unsigned char major, unsigned char minor)
{
    if (!sp_rtrace_options->enable) return 0;
    char* buffer = pipe_buffer_lock(), *ptr = buffer + SP_RTRACE_PROTO_TYPE_SIZE;
    ptr += write_dword(ptr, SP_RTRACE_PROTO_MODULE_INFO);
    ptr += write_dword(ptr, id);
    ptr += write_dword(ptr, (major << 16) | minor);
    ptr += write_string(ptr, name);
    int size = ptr - buffer;
    write_dword(buffer, size - SP_RTRACE_PROTO_TYPE_SIZE);
    pipe_buffer_unlock(buffer, size);
    return size;
}


static int write_resource_registry(const sp_rtrace_resource_t* resource)
{
	if (!sp_rtrace_options->enable) return 0;
	char* buffer = pipe_buffer_lock(), *ptr = buffer + SP_RTRACE_PROTO_TYPE_SIZE;
	ptr += write_dword(ptr, SP_RTRACE_PROTO_RESOURCE_REGISTRY);
	ptr += write_dword(ptr, resource->id);
	ptr += write_dword(ptr, resource->flags);
	ptr += write_string(ptr, resource->type);
	ptr += write_string(ptr, resource->desc);
	int size = ptr - buffer;
	write_dword(buffer, size - SP_RTRACE_PROTO_TYPE_SIZE);
	pipe_buffer_unlock(buffer, size);

	return size;
}


/*
 *
 */

/**
 * Writes the initial handshake packet into processor pipe.
 *
 * @param[in] major   the major protocol version number.
 * @param[in] minor   the minor protocol version number.
 * @param[in] arch    the system architecture.
 * @return            the number of bytes written.
 */
static int write_handshake(int major, int minor, const char* arch)
{
	char* buffer = pipe_buffer_lock(), *ptr = buffer + 2;
	write_byte(buffer, SP_RTRACE_PROTO_HS_ID);
	ptr += write_byte(ptr, major);
	ptr += write_byte(ptr, minor);
	char* pstr = ptr++;
	while (*arch) *ptr++ = *arch++;
	write_byte(pstr, ptr - pstr - 1);
	short endian = 0x0100;
	char endianness = *(char*)&endian;
	ptr += write_byte(ptr, endianness);
	ptr += write_byte(ptr, sizeof(pointer_t));

	int size = ptr - buffer;
	SP_RTRACE_PROTO_ALIGN_SIZE(size);
	write_byte(buffer + 1, size - 2);
	pipe_buffer_unlock(buffer, size);
	return size;
}

/**
 * Writes output settings packet into processor pipe.
 *
 * @return
 */
static int write_output_settings(const char* output_dir, const char* postproc)
{
	char* buffer = pipe_buffer_lock(), *ptr = buffer + SP_RTRACE_PROTO_TYPE_SIZE;
	ptr += write_dword(ptr, SP_RTRACE_PROTO_OUTPUT_SETTINGS);
	if (sp_rtrace_options->manage_preproc) {
		ptr += write_string(ptr, NULL);
		ptr += write_string(ptr, NULL);
	}
	else {
		ptr += write_string(ptr, output_dir);
		ptr += write_string(ptr, postproc);
	}
	int size = ptr - buffer;
	write_dword(buffer, size - SP_RTRACE_PROTO_TYPE_SIZE);
	pipe_buffer_unlock(buffer, size);
	return size;
}

/**
 * Writes process info (PI) packet.
 *
 * @return   the number of bytes written.
 */
static int write_process_info()
{
	char* buffer = pipe_buffer_lock(), *ptr = buffer + SP_RTRACE_PROTO_TYPE_SIZE;
	ptr += write_dword(ptr, SP_RTRACE_PROTO_PROCESS_INFO);
	ptr += write_dword(ptr, getpid());
	/* store empty timestamp, pre-processor must scan
	 * this packet and update timestamp if its zero */
	ptr += write_dword(ptr, 0);
	ptr += write_dword(ptr, 0);
	/* store backtrace depth setting */
	ptr += write_dword(ptr, sp_rtrace_options->backtrace_depth);
	/* store process name */
	ptr += write_string(ptr, proc_name);

	int size = ptr - buffer;
	write_dword(buffer, size - SP_RTRACE_PROTO_TYPE_SIZE);
	pipe_buffer_unlock(buffer, size);
	return size;
}

/**
 * Writes heap information (HI) packet.
 *
 * @return  the number of bytes written.
 */
static int write_heap_info()
{
	if (heap_info.arena) {
		char* buffer = pipe_buffer_lock(), *ptr = buffer + SP_RTRACE_PROTO_TYPE_SIZE;
		ptr += write_dword(ptr, SP_RTRACE_PROTO_HEAP_INFO);
		ptr += write_pointer(ptr, heap_bottom);
		ptr += write_pointer(ptr, (pointer_t)sbrk(0));
		ptr += write_dword(ptr, heap_info.arena);
		ptr += write_dword(ptr, heap_info.ordblks);
		ptr += write_dword(ptr, heap_info.smblks);
		ptr += write_dword(ptr, heap_info.hblks);
		ptr += write_dword(ptr, heap_info.hblkhd);
		ptr += write_dword(ptr, heap_info.usmblks);
		ptr += write_dword(ptr, heap_info.fsmblks);
		ptr += write_dword(ptr, heap_info.uordblks);
		ptr += write_dword(ptr, heap_info.fordblks);
		ptr += write_dword(ptr, heap_info.keepcost);

		int size = ptr - buffer;
		write_dword(buffer, size - SP_RTRACE_PROTO_TYPE_SIZE);
		pipe_buffer_unlock(buffer, size);
		return size;
	}
	return 0;
}



/**
 * Writes initial data packets (HS + MI) into processor pipe
 * and flushed it.
 *
 * @return
 */
static void write_initial_data()
{
    unsigned int i;

	pipe_buffer_reset();
	write_handshake(SP_RTRACE_PROTO_VERSION_MAJOR, SP_RTRACE_PROTO_VERSION_MINOR, BUILD_ARCH);
	write_output_settings(sp_rtrace_options->output_dir, sp_rtrace_options->postproc);
	write_process_info();
	write_module_info(0, module_info.name, module_info.version_major, module_info.version_minor);
	/* write MI packets for all tracing modules */
	for (i = 0; i < rtrace_module_index; i++) {
		rtrace_module_t* module = &rtrace_modules[i];
	    write_module_info(module->id, module->name, module->vmajor, module->vminor);
    }
	/* write resource registry records */
	for (i = 0; i < rtrace_resource_index; i++) {
	    write_resource_registry(&rtrace_resources[i]);
    }

	sp_rtrace_write_new_library("*");
	pipe_buffer_flush();
}

/**
 * Monitor dlopen calls to generate new library (NL) packets.
 */
static void* (*dlopen_rt)(const char*, int);

static void* dlopen_init(const char* library, int flag) {
	dlopen_rt = dlsym(RTLD_NEXT, "dlopen");
	return dlopen(library, flag);
}

static void* (*dlopen_rt)(const char*, int) = dlopen_init;

void* dlopen(const char* library, int flag)
{
	void* handle = dlopen_rt(library, flag);
	if (handle && sp_rtrace_options->enable) {
		sp_rtrace_write_new_library(library);
	}
	return handle;
}

/**
 * SIGUSR1 handler. Enables/disables tracing
 */
static void signal_toggle_tracing(int signo __attribute((unused)))
{
	LOG("enable=%d\n",  !sp_rtrace_options->enable);
	sp_rtrace_options->enable = !sp_rtrace_options->enable;
	if (sp_rtrace_options->enable) {
		fd_proc = open_pipe();
		if (fd_proc > 0) {
			write_initial_data();
			enable_tracing(true);
		}
	}
	else {
		if (fd_proc > 0) {
			sp_rtrace_write_new_library("*");
			write_heap_info();
			enable_tracing(false);
			pipe_buffer_flush();
			close_pipe(fd_proc);
			fd_proc = 0;
		}
	}
}

/**
 * Copies at most size bytes from string src to dst including
 * the trailing null byte.
 *
 * @param[in] dst   the destination buffer.
 * @param[in] src   the source buffer.
 * @param[in] size  size of the destination buffer.
 * @return          a pointer to the terminating null in dst.
 */
static char* _stpncpy(char* dst, const char* src, int size)
{
	char* tail = dst;
	while ( dst - tail < size - 1 && (*dst = *src)) {
		dst++;
		src++;
	}
	*dst = '\0';
	return dst;
}

/**
 * Converts string to an integer value.
 * @param str
 * @return
 */
static int _atoi(const char* str)
{
	int value = 0;
	while (*str && *str >= '0' && *str <= '9') {
		value = value * 10 + *str++ - '0';
	}
	return value;
}

/**
 * Converts integer value to a string.
 *
 * @param[in] buffer  the output buffer.
 * @param[in] value   the value to convert.
 * @return            the output buffer.
 */
static char* _itoa(char* buffer, int value)
{
	char tmp[16], *ptr = tmp;
	char* ptr_buffer = buffer;

	while (true) {
		*ptr = value % 10 + '0';
		value /= 10;
		if (value == 0) break;
		ptr++;
	}
	do {
		*ptr_buffer++ = *ptr--;
	} while (ptr >= tmp);
	*ptr_buffer = '\0';
	return buffer;
}

/*
 * Public API implementation
 */

int sp_rtrace_write_new_library(const char* library)
{
	char* buffer = pipe_buffer_lock(), *ptr = buffer + SP_RTRACE_PROTO_TYPE_SIZE;
	ptr += write_dword(ptr, SP_RTRACE_PROTO_NEW_LIBRARY);
	ptr += write_string(ptr, library);
	int size = ptr - buffer;
	write_dword(buffer, size - SP_RTRACE_PROTO_TYPE_SIZE);
	pipe_buffer_unlock(buffer, size);
	return size;
}

int sp_rtrace_write_attachment(const sp_rtrace_attachment_t* file)
{
	char* buffer = pipe_buffer_lock(), *ptr = buffer + SP_RTRACE_PROTO_TYPE_SIZE;
	ptr += write_dword(ptr, SP_RTRACE_PROTO_ATTACHMENT);
	ptr += write_string(ptr, file->name);
	ptr += write_string(ptr, file->path);
	int size = ptr - buffer;
	write_dword(buffer, size - SP_RTRACE_PROTO_TYPE_SIZE);
	pipe_buffer_unlock(buffer, size);
	return size;
}

int sp_rtrace_write_context_registry(sp_rtrace_context_t* context)
{
	if (!sp_rtrace_options->enable) return 0;
	char* buffer = pipe_buffer_lock(), *ptr = buffer + SP_RTRACE_PROTO_TYPE_SIZE;
	ptr += write_dword(ptr, SP_RTRACE_PROTO_CONTEXT_REGISTRY);
	ptr += write_dword(ptr, context->id);
	ptr += write_string(ptr, context->name);
	int size = ptr - buffer;
	write_dword(buffer, size - SP_RTRACE_PROTO_TYPE_SIZE);
	pipe_buffer_unlock(buffer, size);
	return size;
}

int sp_rtrace_write_function_call(sp_rtrace_fcall_t* call, sp_rtrace_ftrace_t* trace, sp_rtrace_farg_t* args)
{
	if (!sp_rtrace_options->enable) return 0;

	/* check if the resource type contains resource identifier */
	if (call->res_type_flag != SP_RTRACE_FCALL_RFIELD_ID) {
		return -EINVAL;
	}

	int size = 0;
	pointer_t bt_frames[256];
	sp_rtrace_ftrace_t trace_data = {
			.nframes = 0,
			.frames = bt_frames + BT_SKIP_TOP,
	};

	if (!trace && sp_rtrace_options->backtrace_depth && (call->type == SP_RTRACE_FTYPE_ALLOC || sp_rtrace_options->backtrace_all)) {
		unsigned int bt_depth = sp_rtrace_options->backtrace_depth + BT_SKIP_TOP + BT_SKIP_BOTTOM;
		if (bt_depth > sizeof(bt_frames) / sizeof(bt_frames[0])) {
			bt_depth = sizeof(bt_frames) / sizeof(bt_frames[0]);
		}
		/* backtrace() function could trigger tracked function calls.
		 * lock those functions for other threads while using the standard
		 * functions for the current thread  */
		if (backtrace_lock) {
			fprintf(stderr, "ERROR: infinite recursion detected backtrace() calling %s()\n", call->name);
			exit (-1);
		}
		backtrace_lock = 1;
		trace_data.nframes = backtrace_impl((void**)bt_frames, bt_depth);
		backtrace_lock = 0;

		trace_data.nframes -= BT_SKIP_TOP + BT_SKIP_BOTTOM;
		if ((int)trace_data.nframes > sp_rtrace_options->backtrace_depth) {
			trace_data.nframes = sp_rtrace_options->backtrace_depth;
		}
		if (trace_data.nframes > 0) {
			trace = &trace_data;
		}
	}
	char* buffer = pipe_buffer_lock(), *ptr = buffer + SP_RTRACE_PROTO_TYPE_SIZE;

	/* write FC packet */
	ptr += write_dword(ptr, SP_RTRACE_PROTO_FUNCTION_CALL);

	ptr += write_dword(ptr, (unsigned long)call->res_type);

	ptr += write_dword(ptr, sp_rtrace_get_call_context());

	int timestamp = 0;
	if (sp_rtrace_options->enable_timestamps) {
		struct timespec ts;
		if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
			timestamp = ts.tv_nsec / 1000000 + ts.tv_sec % (60 * 60 * 24) * 1000;
		}
	}
	ptr += write_dword(ptr, timestamp);
	ptr += write_dword(ptr, call->type);
	ptr += write_string(ptr, call->name);
	ptr += write_dword(ptr, call->res_size);
	ptr += write_pointer(ptr, call->res_id);
	size = ptr - buffer;
	write_dword(buffer, size - SP_RTRACE_PROTO_TYPE_SIZE);

	/* write FA packet */
	if (args) {
		char* psize = ptr;
		ptr += SP_RTRACE_PROTO_TYPE_SIZE;
		ptr += write_dword(ptr, SP_RTRACE_PROTO_FUNCTION_ARGS);
		char* pargs = ptr;
		ptr += sizeof(int);
		const sp_rtrace_farg_t* first_arg = args;
		while (args->name) {
			ptr += write_string(ptr, args->name);
			ptr += write_string(ptr, args->value);
			args++;
		}
		write_dword(pargs, args - first_arg);
		size = ptr - psize;
		write_dword(psize, size - SP_RTRACE_PROTO_TYPE_SIZE);
	}

	/* write BT packet */
	/* strip the unnecessary frames from the top and bottom */
	if (trace && trace->nframes) {
		unsigned int i;
		ptr += write_dword(ptr, sizeof(int) * 2 + sizeof(pointer_t) * trace->nframes);
		ptr += write_dword(ptr, SP_RTRACE_PROTO_BACKTRACE);
		ptr += write_dword(ptr, trace->nframes);
		for (i = 0; i < trace->nframes; i++) {
			ptr += write_pointer(ptr, (pointer_t)trace->frames[i]);
		}
		size += ptr - buffer - size;
	}
	else {
		/* write empty backtrace packet */
		ptr += write_dword(ptr, sizeof(int) * 2);
		ptr += write_dword(ptr, SP_RTRACE_PROTO_BACKTRACE);
		ptr += write_dword(ptr, 0);
		size += ptr - buffer - size;
	}
	pipe_buffer_unlock(buffer, size);
	return size;
}

unsigned int sp_rtrace_register_module(const char* name, unsigned char vmajor, unsigned char vminor, sp_rtrace_enable_tracing_t enable_func)
{
	if (rtrace_module_index >= sizeof(rtrace_modules) / sizeof(rtrace_modules[0])) {
		return 0;
	}
	rtrace_module_t* module = &rtrace_modules[rtrace_module_index];
	module->enable = enable_func;
	module->vmajor = vmajor;
	module->vminor = vminor;
    module->name = name;
    module->id = (1 << rtrace_module_index++);
    module->enable(sp_rtrace_options->enable);

	/* If the tracing has been already enabled,
	 *  write module info packet for the registered module */
	if (sp_rtrace_options->enable) {
		write_module_info(module->id, name, vmajor, vminor);
	}
	return module->id;
}

unsigned int sp_rtrace_register_resource(sp_rtrace_resource_t* resource)
{
	/* Check if the specified resource type is already registered. If so return
	 * the registered id. */
	unsigned int i;
	for (i = 0; i < rtrace_resource_index; i++) {
	   if (!strcmp(rtrace_resources[i].type, resource->type)) {
		   resource->id = rtrace_resources[i].id;
		   return resource->id;
	   }
    }

	/* Register new resource type */
	if (rtrace_resource_index >= sizeof(rtrace_resources) / sizeof(rtrace_resources[0])) {
		return -1;
	}
    ((sp_rtrace_resource_t*)resource)->id = rtrace_resource_index + 1;
	rtrace_resources[rtrace_resource_index++] = *resource;
	if (sp_rtrace_options->enable) {
		write_resource_registry(resource);
	}
	return resource->id;
}


void sp_rtrace_store_heap_info()
{
	heap_info = mallinfo();
}

bool sp_rtrace_initialize()
{
	static volatile int initialize_lock = 0;
	if (sync_bool_compare_and_swap(&initialize_lock, 0, 1)) {

		/* cache the heap bottom address */
		heap_bottom = (pointer_t)sbrk(0);

		/* first check if the environment is ready */
		const char* env_ready = getenv(SP_RTRACE_READY);
		if (!env_ready) {
			/* environment options aren't accessible (yet) */
			initialize_lock = 0;
			return false;
		}

		LOG("initializing... (pid=%d)", getpid());
		/* reset the scratchbox preload configuration */
		if (query_scratchbox()) {
			unlink("/etc/ld.so.preload");
		}

		/* Looks like there are conflicts when calling dlsym (which is done by sp_rtrace_init_context
		 * from audit libraries. As context is not supported by audit anyway, just skip context
		 * support initialization.
		 * TODO: investigate what's exactly happening.
		 */
		if (!getenv(rtrace_env_opt[OPT_AUDIT])) {
			/* Initialize call context support. */
			sp_rtrace_init_context();
		}

		/* read process name */
		get_proc_name(proc_name, sizeof(proc_name));

		/* get the pre-processor named pipe name */
		char* ptr = _stpncpy(pipe_path, SP_RTRACE_PIPE_PATTERN, sizeof(pipe_path));
		_itoa(ptr, getpid());

		/* read backtrace depth */
		const char* env_backtrace_depth = getenv(rtrace_env_opt[OPT_BACKTRACE_DEPTH]);
		if (env_backtrace_depth && *env_backtrace_depth) {
			sp_rtrace_options->backtrace_depth = _atoi(env_backtrace_depth);
			LOG("backtrace_depth=%d", sp_rtrace_options->backtrace_depth);
		}

		/* read timestamp option */
		const char* env_disable_timestamps = getenv(rtrace_env_opt[OPT_DISABLE_TIMESTAMPS]);
		if (env_disable_timestamps && *env_disable_timestamps == '1') {
			sp_rtrace_options->enable_timestamps = false;
			LOG("enable_timestamps=%d", sp_rtrace_options->enable_timestamps);
		}

		/* read disable packet buffering option */
		const char* env_disable_packet_buffering = getenv(rtrace_env_opt[OPT_DISABLE_PACKET_BUFFERING]);
		if (env_disable_packet_buffering && *env_disable_packet_buffering == '1') {
			sp_rtrace_options->enable_packet_buffering = false;
			LOG("enable_packet_buffering=%d", sp_rtrace_options->enable_packet_buffering);
		}

		/* read manage-preproc option */
		const char* env_manage_preproc = getenv(rtrace_env_opt[OPT_MANAGE_PREPROC]);
		if (env_manage_preproc && *env_manage_preproc == '1') {
			sp_rtrace_options->manage_preproc = true;
			LOG("manage_preproc=%d", sp_rtrace_options->manage_preproc);
		}

		/* read backtrace-all option */
		const char* env_backtrace_all = getenv(rtrace_env_opt[OPT_BACKTRACE_ALL]);
		if (env_backtrace_all && *env_backtrace_all == '1') {
			sp_rtrace_options->backtrace_all = true;
			LOG("backtrace_all=%d", sp_rtrace_options->backtrace_all);
		}

		/* read post-processor options */
		const char* env_postproc = getenv(rtrace_env_opt[OPT_POSTPROC]);
		if (env_postproc) {
			_stpncpy(sp_rtrace_options->postproc, env_postproc, sizeof(sp_rtrace_options->postproc));
			LOG("posptproc=%s", sp_rtrace_options->postproc);
		}

		/* read output directory option */
		const char* env_output_dir = getenv(rtrace_env_opt[OPT_OUTPUT_DIR]);
		if (env_output_dir) {
			_stpncpy(sp_rtrace_options->output_dir, env_output_dir, sizeof(sp_rtrace_options->output_dir));
			LOG("output_dir=%s", sp_rtrace_options->output_dir);
		}

		/* read tracing enable option */
		const char* env_enable = getenv(rtrace_env_opt[OPT_START]);
		if (env_enable && *env_enable == '1') {
			sp_rtrace_options->enable = true;
		}
		
		/* read libunwind setting */
		const char* env_libunwind = getenv(rtrace_env_opt[OPT_LIBUNWIND]);
		if (env_libunwind && *env_libunwind == '1') {
			LOG("Use libunwind for stack frame unwinding");
			backtrace_impl = libunwind_initialize();
			if (backtrace_impl == NULL) {
				fprintf(stderr, "WARNING: libunwind backtracing option specified, but libunwind_unitialize returned NULL. "
						"Switching to standard backtrace() implementation.\n");
				backtrace_impl = backtrace;
			}
		}

		/* enable tracing if needed */
		if (sp_rtrace_options->enable) {
			fd_proc = open_pipe();
			write_initial_data();
			enable_tracing(true);
		}

		initialize_lock = 2;
		return true;
	}
	return true;
}

void sp_rtrace_get_out_filename(const char* pattern, char* buffer, size_t size)
{
	char* ptr = buffer;
	if (*sp_rtrace_options->output_dir) _stpncpy(buffer, sp_rtrace_options->output_dir, size);
	else *ptr++ = '.';
	*ptr++ = '/';
	ptr = _stpncpy(ptr, pattern, size - (ptr - buffer));
	*ptr++ = '-';
	int index = 0;
	do {
		_itoa(ptr, index++);
	} while (access(buffer, F_OK) == 0);
}


int sp_rtrace_copy_file(const char* source, const char* dest)
{
	int rc = 0;
	int fd_in = open(source, O_RDONLY);
	if (fd_in == -1) return -errno;
	int fd_out = creat(dest, 0644);
	if (fd_out == -1) {
		rc = -errno;
		close(fd_in);
		return rc;
	}
	char buffer[0x8000];
	int n_in;
	while ( (n_in = read(fd_in, buffer, sizeof(buffer))) ) {
		int n_out = write(fd_out, buffer, n_in);
		if (n_in != n_out) {
			rc = -errno;
			break;
		}
	}
	close(fd_in);
	close(fd_out);
	return rc;
}


static void trace_main_init(void) __attribute__((constructor));
static void trace_main_fini(void) __attribute__((destructor));

/**
 * Initializes the main tracing module
 */
static void trace_main_init(void)
{
	sp_rtrace_initialize();

	int toggle_signal = SIGUSR1;

	/* read toggle signal number */
	const char* env_toggle_signal = getenv(rtrace_env_opt[OPT_TOGGLE_SIGNAL]);
	if (env_toggle_signal) {
		int sig = atoi(env_toggle_signal);
		if (sig) toggle_signal = sig;
		LOG("toggle_signal=%d", toggle_signal);
	}

	struct sigaction sa = {.sa_handler = signal_toggle_tracing};
	sigemptyset(&sa.sa_mask);
	if (sigaction(toggle_signal, &sa, NULL) == -1) {
		fprintf(stderr, "ERROR: failed to set signal %d\n", toggle_signal);
		exit (-1);
	}
}

static void trace_main_fini(void)
{
	if (fd_proc > 0) {
		if (sp_rtrace_options->enable) {
			sp_rtrace_write_new_library("*");
			write_heap_info();
		}
		enable_tracing(false);
		pipe_buffer_flush();
		close_pipe(fd_proc);
	}
}


