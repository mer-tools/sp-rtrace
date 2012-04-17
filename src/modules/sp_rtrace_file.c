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

/**
 * @file sp_rtrace_file.c
 *
 * File operation tracking module implementation.
 */
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/inotify.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <unistd.h>

#include "sp_rtrace_main.h"
#include "sp_rtrace_module.h"

#include "common/sp_rtrace_proto.h"

/* Module information */
static sp_rtrace_module_info_t module_info = {
		.type = MODULE_TYPE_PRELOAD,
		.version_major = 1,
		.version_minor = 0,
		.name = "file",
		.description = "File operation tracking module. "
				       "Tracks file opening/closing operations.",
};

static sp_rtrace_resource_t res_fd = {
		.type = "fd",
		.desc = "file descriptor",
		.flags = SP_RTRACE_RESOURCE_DEFAULT,
};

static sp_rtrace_resource_t res_fp = {
		.type = "fp",
		.desc = "file pointer",
		.flags = SP_RTRACE_RESOURCE_DEFAULT,
};


/* required for glibc < v2.11 */
#ifndef F_GETOWN_EX
 #define F_GETOWN_EX	16
#endif

#ifndef F_SETOWN_EX
 #define F_SETOWN_EX	15
#endif

 /*
  * file module function set
  */

typedef int (*open_t)(const char* pathname, int flags, ...);
typedef int (*open64_t)(const char* pathname, int flags, ...);
typedef int (*close_t)(int fd);
typedef int (*dup2_t)(int oldfd, int newfd);
typedef int (*socket_t)(int domain, int type, int protocol);
typedef FILE* (*fopen_t)(const char *path, const char *mode);
typedef FILE* (*fdopen_t)(int fd, const char *mode);
typedef FILE* (*freopen_t)(const char *path, const char *mode, FILE *stream);
typedef int (*fclose_t)(FILE *fp);
typedef int (*fcloseall_t)(void);
typedef int (*creat_t)(const char *pathname, mode_t mode);
typedef int (*accept_t)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
typedef int (*dup_t)(int oldfd);
typedef int (*fcntl_t)(int fd, int cmd, ...);
typedef int (*socketpair_t)(int domain, int type, int protocol, int sv[2]);
typedef int (*inotify_init_t)(void);
typedef int (*pipe_t)(int pipefd[2]);
typedef int (*pipe2_t)(int pipefd[2], int flags);


typedef struct {
	fopen_t fopen;
	fdopen_t fdopen;
	freopen_t freopen;
	fclose_t fclose;
	fcloseall_t fcloseall;
	open_t open;
	open64_t open64;
	close_t close;
	dup2_t dup2;
	socket_t socket;
	creat_t creat;
	accept_t accept;
	dup_t dup;
	fcntl_t fcntl;
	socketpair_t socketpair;
	inotify_init_t inotify_init;
	pipe_t pipe;
	pipe2_t pipe2;
} trace_t;

/* original function references */
static trace_t trace_off;
/* tracing function references */
static trace_t trace_on;
/* tracing function initializers */
static trace_t trace_init;

/* Runtime function references */
static trace_t* trace_rt = &trace_init;

/* Initialization runtime function references */
static trace_t* trace_init_rt = &trace_off;

/**
 * Enables/disables tracing.
 *
 * @param[in] value   enable tracing if true, otherwise disable it.
 * @return
 */
static void enable_tracing(bool value)
{
	trace_rt = value ? &trace_on : &trace_off;
}

/**
 * Initializes original function references.
 *
 * @return
 */
static void trace_initialize(void)
{
	static int init_mode = MODULE_UNINITIALIZED;
	switch (init_mode) {
		case MODULE_UNINITIALIZED: {
			trace_off.open = (open_t)dlsym(RTLD_NEXT, "open");
			trace_off.open64 = (open_t)dlsym(RTLD_NEXT, "open64");
			trace_off.close = (close_t)dlsym(RTLD_NEXT, "close");
			trace_off.dup2 = (dup2_t)dlsym(RTLD_NEXT, "dup2");
			trace_off.socket = (socket_t)dlsym(RTLD_NEXT, "socket");
			trace_off.fopen = (fopen_t)dlsym(RTLD_NEXT, "fopen");
			trace_off.fdopen = (fdopen_t)dlsym(RTLD_NEXT, "fdopen");
			trace_off.freopen = (freopen_t)dlsym(RTLD_NEXT, "freopen");
			trace_off.fclose = (fclose_t)dlsym(RTLD_NEXT, "fclose");
			trace_off.fcloseall = (fcloseall_t)dlsym(RTLD_NEXT, "fcloseall");
			trace_off.creat = (creat_t)dlsym(RTLD_NEXT, "creat");
			trace_off.accept = (accept_t)dlsym(RTLD_NEXT, "accept");
			trace_off.dup = (dup_t)dlsym(RTLD_NEXT, "dup");
			trace_off.fcntl = (fcntl_t)dlsym(RTLD_NEXT, "fcntl");
			trace_off.socketpair = (socketpair_t)dlsym(RTLD_NEXT, "socketpair");
			trace_off.inotify_init = (inotify_init_t)dlsym(RTLD_NEXT, "inotify_init");
			trace_off.pipe = (pipe_t)dlsym(RTLD_NEXT, "pipe");
			trace_off.pipe2 = (pipe2_t)dlsym(RTLD_NEXT, "pipe2");
			init_mode = MODULE_LOADED;

			LOG("module loaded: %s (%d.%d)", module_info.name, module_info.version_major, module_info.version_minor);
		}

		case MODULE_LOADED: {
			if (sp_rtrace_initialize() && init_mode == MODULE_LOADED) {
				init_mode = MODULE_READY;

				sp_rtrace_register_module(module_info.name, module_info.version_major, module_info.version_minor, enable_tracing);
				sp_rtrace_register_resource(&res_fd);
				sp_rtrace_register_resource(&res_fp);
				trace_init_rt = trace_rt;

				LOG("module ready: %s (%d.%d)", module_info.name, module_info.version_major, module_info.version_minor);
			}
		}
	}
}

/*
 * tracing functions
 */
static int trace_open(const char* pathname, int flags, ...)
{
	int rc = 0;
	if (flags & O_CREAT) {
		va_list args;
		va_start(args, flags);
		rc = trace_off.open(pathname, flags, va_arg(args, int));
		va_end(args);
	}
	else {
		rc = trace_off.open(pathname, flags);
	}
	backtrace_lock = 0;

	if (rc != -1) {
		char flags_s[16];
		sprintf(flags_s, "%x", flags);

		sp_rtrace_fcall_t call = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "open",
				.res_size = 1,
				.res_id = (pointer_t)rc,
		};
		sp_rtrace_farg_t args[] = {
				{.name = "pathname", .value = (char*)pathname},
				{.name = "flags", .value = flags_s},
				{0}
		};
		sp_rtrace_write_function_call(&call, NULL, args);
	}
	return rc;
}

/*
 * tracing functions
 */
static int trace_open64(const char* pathname, int flags, ...)
{
	int rc = 0;
	if (flags & O_CREAT) {
		va_list args;
		va_start(args, flags);
		rc = trace_off.open64(pathname, flags, va_arg(args, int));
		va_end(args);
	}
	else {
		rc = trace_off.open64(pathname, flags);
	}
	backtrace_lock = 0;

	if (rc != -1) {
		char flags_s[16];
		sprintf(flags_s, "%x", flags);

		sp_rtrace_fcall_t call = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "open64",
				.res_size = 1,
				.res_id = (pointer_t)rc,
		};
		sp_rtrace_farg_t args[] = {
				{.name = "pathname", .value = (char*)pathname},
				{.name = "flags", .value = flags_s},
				{0}
		};
		sp_rtrace_write_function_call(&call, NULL, args);
	}
	return rc;
}


static int trace_close(int fd)
{
	int rc = trace_off.close(fd);
	backtrace_lock = 0;
	if (rc != -1) {
		sp_rtrace_fcall_t call = {
				.type = SP_RTRACE_FTYPE_FREE,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "close",
				.res_size = 0,
				.res_id = (pointer_t)fd,
		};
		sp_rtrace_write_function_call(&call, NULL, NULL);
	}
	return rc;
}

static int trace_dup2(int oldfd, int newfd)
{
	int rc = trace_off.dup2(oldfd, newfd);
	if (rc != -1) {
		sp_rtrace_fcall_t call1 = {
				.type = SP_RTRACE_FTYPE_FREE,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "dup2",
				.res_size = 0,
				.res_id = (pointer_t)newfd,
		};
		sp_rtrace_write_function_call(&call1, NULL, NULL);

		sp_rtrace_fcall_t call2 = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "dup2",
				.res_size = 1,
				.res_id = (pointer_t)rc,
		};
		sp_rtrace_write_function_call(&call2, NULL, NULL);
	}
	return rc;
}

static int trace_socket(int domain, int type, int protocol)
{
	int rc = trace_off.socket(domain, type, protocol);
	if (rc != -1) {
		char domain_s[64], type_s[64], protocol_s[64];
		snprintf(domain_s, sizeof(domain_s), "0x%x", domain);
		snprintf(type_s, sizeof(type_s), "0x%x", type);
		snprintf(protocol_s, sizeof(protocol_s), "0x%x", protocol);

		sp_rtrace_fcall_t call = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "socket",
				.res_size = 1,
				.res_id = (pointer_t)rc,
		};
		sp_rtrace_farg_t args[] = {
				{.name = "domain", .value = domain_s},
				{.name = "type", .value = type_s},
				{.name = "protocol", .value = protocol_s},
				{0}
		};
		sp_rtrace_write_function_call(&call, NULL, args);
	}
	return rc;
}

static FILE *trace_fopen(const char *path, const char *mode)
{
	FILE* rc = trace_off.fopen(path, mode);
	if (rc) {
		sp_rtrace_fcall_t call = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fp.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "fopen",
				.res_size = 1,
				.res_id = (pointer_t)rc,
		};
		sp_rtrace_farg_t args[] = {
				{.name = "path", .value = (char*)path},
				{.name = "mode", .value = (char*)mode},
				{0}
		};
		sp_rtrace_write_function_call(&call, NULL, args);

	}
	return rc;
}

static FILE *trace_fdopen(int fd, const char *mode)
{
	FILE* rc = trace_off.fdopen(fd, mode);
	if (rc) {
		char fd_s[32];
		sprintf(fd_s, "%d", fd);
		sp_rtrace_fcall_t call1 = {
				.type = SP_RTRACE_FTYPE_FREE,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "fdopen",
				.res_size = 0,
				.res_id = (pointer_t)fd,
		};
		sp_rtrace_write_function_call(&call1, NULL, NULL);

		sp_rtrace_fcall_t call2 = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fp.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "fdopen",
				.res_size = 1,
				.res_id = (pointer_t)rc,
		};
		sp_rtrace_farg_t args[] = {
				{.name = "fd", .value = fd_s},
				{.name = "mode", .value = (char*)mode},
				{0}
		};
		sp_rtrace_write_function_call(&call2, NULL, args);

	}
	return rc;
}

static FILE *trace_freopen(const char *path, const char *mode, FILE *stream)
{
	FILE* rc = trace_off.freopen(path, mode, stream);
	if (rc) {
		sp_rtrace_fcall_t call1 = {
				.type = SP_RTRACE_FTYPE_FREE,
				.res_type = (void*)res_fp.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "freopen",
				.res_size = 0,
				.res_id = (pointer_t)stream,
		};
		sp_rtrace_write_function_call(&call1, NULL, NULL);

		sp_rtrace_fcall_t call2 = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fp.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "freopen",
				.res_size = 1,
				.res_id = (pointer_t)rc,
		};
		sp_rtrace_farg_t args[] = {
				{.name = "path", .value = (char*)path},
				{.name = "mode", .value = (char*)mode},
				{0}
		};
		sp_rtrace_write_function_call(&call2, NULL, args);

	}
	return rc;
}

static int trace_fclose(FILE *fp)
{
	int rc = trace_off.fclose(fp);
	if (rc == 0) {
		sp_rtrace_fcall_t call = {
				.type = SP_RTRACE_FTYPE_FREE,
				.res_type = (void*)res_fp.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "fclose",
				.res_size = 0,
				.res_id = (pointer_t)fp,
		};
		sp_rtrace_write_function_call(&call, NULL, NULL);
	}
	return rc;
}

static int trace_fcloseall(void)
{
	int rc = trace_off.fcloseall();
	if (rc == 0) {
		sp_rtrace_fcall_t call = {
				.type = SP_RTRACE_FTYPE_FREE,
				.res_type = (void*)res_fp.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "fcloseall",
				.res_size = 0,
				.res_id = (pointer_t)-1,
		};
		sp_rtrace_write_function_call(&call, NULL, NULL);
	}
	return rc;
}

static int trace_creat(const char *pathname, mode_t mode)
{
	int rc = trace_off.creat(pathname, mode);
	if (rc != -1) {
		char mode_s[16];
		sprintf(mode_s, "0x%x", mode);

		sp_rtrace_fcall_t call = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "creat",
				.res_size = 1,
				.res_id = (pointer_t)rc,
		};
		sp_rtrace_farg_t args[] = {
				{.name = "pathname", .value = (char*)pathname},
				{.name = "mode", .value = mode_s},
				{0}
		};
		sp_rtrace_write_function_call(&call, NULL, args);
	}
	return rc;
}

static int trace_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int rc = trace_off.accept(sockfd, addr, addrlen);
	if (rc != -1) {
		sp_rtrace_fcall_t call = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "accept",
				.res_size = 1,
				.res_id = (pointer_t)rc,
		};
		sp_rtrace_write_function_call(&call, NULL, NULL);
	}
	return rc;
}

static int trace_dup(int oldfd)
{
	int rc = trace_off.dup(oldfd);
	if (rc != -1) {
		sp_rtrace_fcall_t call = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "dup",
				.res_size = 1,
				.res_id = (pointer_t)rc,
		};
		sp_rtrace_write_function_call(&call, NULL, NULL);
	}
	return rc;
}

static int trace_fcntl(int fd, int cmd, ...)
{
	int rc;
	switch (cmd) {
		case F_DUPFD:
		case F_DUPFD_CLOEXEC:
		case F_SETFD:
		case F_SETFL:
		case F_SETLK:
		case F_SETLKW:
		case F_GETLK:
		case F_SETOWN:
		case F_GETOWN_EX:
		case F_SETOWN_EX:
		case F_SETSIG:
		case F_SETLEASE:
		case F_NOTIFY:
//		case F_SETPIPE_SZ:
		{
			va_list args;
			va_start(args, cmd);
			long newfd = va_arg(args, long);
			va_end(args);
			rc = trace_off.fcntl(fd, cmd, newfd);
			if ( (cmd == F_DUPFD || cmd == F_DUPFD_CLOEXEC) && rc != -1) {
				sp_rtrace_fcall_t call1 = {
						.type = SP_RTRACE_FTYPE_FREE,
						.res_type = (void*)res_fd.id,
						.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
						.name = "fcntl",
						.res_size = 0,
						.res_id = (pointer_t)newfd,
				};
				sp_rtrace_write_function_call(&call1, NULL, NULL);

				sp_rtrace_fcall_t call2 = {
						.type = SP_RTRACE_FTYPE_ALLOC,
						.res_type = (void*)res_fd.id,
						.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
						.name = "fcntl",
						.res_size = 1,
						.res_id = (pointer_t)rc,
				};
				sp_rtrace_write_function_call(&call2, NULL, NULL);
			}
			break;
		}

		default: {
			rc = trace_off.fcntl(fd, cmd);
			break;
		}
	}

	return rc;
}

static int trace_socketpair(int domain, int type, int protocol, int sv[2])
{
	int rc = trace_off.socketpair(domain, type, protocol, sv);
	if (rc != -1) {
		char domain_s[64], type_s[64], protocol_s[64];
		snprintf(domain_s, sizeof(domain_s), "0x%x", domain);
		snprintf(type_s, sizeof(type_s), "0x%x", type);
		snprintf(protocol_s, sizeof(protocol_s), "0x%x", protocol);

		sp_rtrace_farg_t args[] = {
				{.name = "domain", .value = domain_s},
				{.name = "type", .value = type_s},
				{.name = "protocol", .value = protocol_s},
				{0}
		};

		sp_rtrace_fcall_t call1 = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "socketpair",
				.res_size = 1,
				.res_id = (pointer_t)sv[0],
		};
		sp_rtrace_write_function_call(&call1, NULL, args);

		sp_rtrace_fcall_t call2 = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "socketpair",
				.res_size = 1,
				.res_id = (pointer_t)sv[1],
		};
		sp_rtrace_write_function_call(&call2, NULL, args);
	}
	return rc;
}

static int trace_inotify_init(void)
{
	int rc = trace_off.inotify_init();
	if (rc != -1) {
		sp_rtrace_fcall_t call = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "inotify_init",
				.res_size = 1,
				.res_id = (pointer_t)rc,
		};
		sp_rtrace_write_function_call(&call, NULL, NULL);
	}
	return rc;
}

static int trace_pipe(int pipefd[2])
{
	int rc = trace_off.pipe(pipefd);
	if (rc != -1) {
		sp_rtrace_fcall_t call1 = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "pipe",
				.res_size = 1,
				.res_id = (pointer_t)pipefd[0],
		};
		sp_rtrace_write_function_call(&call1, NULL, NULL);

		sp_rtrace_fcall_t call2 = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "pipe",
				.res_size = 1,
				.res_id = (pointer_t)pipefd[1],
		};
		sp_rtrace_write_function_call(&call2, NULL, NULL);
	}
	return rc;
}

static int trace_pipe2(int pipefd[2], int flags)
{
	int rc = trace_off.pipe2(pipefd, flags);
	if (rc != -1) {
		sp_rtrace_fcall_t call1 = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "pipe2",
				.res_size = 1,
				.res_id = (pointer_t)pipefd[0],
		};
		sp_rtrace_write_function_call(&call1, NULL, NULL);

		sp_rtrace_fcall_t call2 = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type = (void*)res_fd.id,
				.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
				.name = "pipe2",
				.res_size = 1,
				.res_id = (pointer_t)pipefd[1],
		};
		sp_rtrace_write_function_call(&call2, NULL, NULL);
	}
	return rc;
}


static trace_t trace_on = {
	.open = trace_open,
	.open64 = trace_open64,
	.close = trace_close,
	.dup2 = trace_dup2,
	.socket = trace_socket,
	.fopen = trace_fopen,
	.fdopen = trace_fdopen,
	.freopen = trace_freopen,
	.fclose = trace_fclose,
	.fcloseall = trace_fcloseall,
	.creat = trace_creat,
	.accept = trace_accept,
	.dup = trace_dup,
	.fcntl = trace_fcntl,
	.socketpair = trace_socketpair,
	.inotify_init = trace_inotify_init,
	.pipe = trace_pipe,
	.pipe2 = trace_pipe2,
};


/* target functions */

int open(const char* pathname, int flags, ...)
{
	int rc;
	if (flags & O_CREAT) {
		int mode;
		va_list args;
		va_start(args, flags);
		mode = va_arg(args, int);
		va_end(args);
		BT_EXECUTE_LOCKED(rc = trace_rt->open(pathname, flags, va_arg(args, int)), trace_off.open(pathname, flags, mode));
	}
	else {
		BT_EXECUTE_LOCKED(rc = trace_rt->open(pathname, flags), trace_off.open(pathname, flags));
	}
	return rc;
}

int open64(const char* pathname, int flags, ...)
{
	int rc;
	if (flags & O_CREAT) {
		int mode;
		va_list args;
		va_start(args, flags);
		mode = va_arg(args, int);
		va_end(args);
		BT_EXECUTE_LOCKED(rc = trace_rt->open64(pathname, flags, va_arg(args, int)), trace_off.open(pathname, flags, mode));
	}
	else {
		BT_EXECUTE_LOCKED(rc = trace_rt->open64(pathname, flags), trace_off.open(pathname, flags));
	}
	return rc;
}

int close(int fd)
{
	/* synchronize allocation functions used by backtrace */
	int rc;
	BT_EXECUTE_LOCKED(rc = trace_rt->close(fd), trace_off.close(fd));
	return rc;
}

int dup2(int oldfd, int newfd)
{
	return trace_rt->dup2(oldfd, newfd);
}

int socket(int domain, int type, int protocol)
{
	return trace_rt->socket(domain, type, protocol);
}

FILE *fopen(const char *path, const char *mode)
{
	return trace_rt->fopen(path, mode);
}

FILE *fdopen(int fd, const char *mode)
{
	return trace_rt->fdopen(fd, mode);
}

FILE *freopen(const char *path, const char *mode, FILE *stream)
{
	return trace_rt->freopen(path, mode, stream);
}

int fclose(FILE *fp)
{
	return trace_rt->fclose(fp);
}

int fcloseall(void)
{
	return trace_rt->fcloseall();
}

int creat(const char *pathname, mode_t mode)
{
	return trace_rt->creat(pathname, mode);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	return trace_rt->accept(sockfd, addr, addrlen);
}

int dup(int oldfd)
{
	return trace_rt->dup(oldfd);
}

int fcntl(int fd, int cmd, ...)
{
	int rc;
	switch (cmd) {
		case F_DUPFD:
		case F_DUPFD_CLOEXEC:
		case F_SETFD:
		case F_SETFL:
		case F_SETLK:
		case F_SETLKW:
#ifndef __amd64__
		case F_SETLK64:
		case F_SETLKW64:
#endif
		case F_GETLK:
		case F_SETOWN:
		case F_GETOWN_EX:
		case F_SETOWN_EX:
		case F_SETSIG:
		case F_SETLEASE:
		case F_NOTIFY:
//		case F_SETPIPE_SZ:
		{
			va_list args;
			va_start(args, cmd);
			rc = trace_rt->fcntl(fd, cmd, va_arg(args, long));
			va_end(args);
			break;
		}

		case F_GETFD:
		case F_GETFL:
		case F_GETOWN:
		case F_GETSIG:
		case F_GETLEASE:
		{
			rc = trace_rt->fcntl(fd, cmd);
			break;
		}

		default: {
			fprintf(stderr, "ERROR: Unknown fcntl command: %d (%x)\n", cmd, cmd);
			exit (-1);
		}
	}
	return rc;
}

int socketpair(int domain, int type, int protocol, int sv[2])
{
	return trace_rt->socketpair(domain, type, protocol, sv);
}

int inotify_init(void)
{
	return trace_rt->inotify_init();
}

int pipe(int pipefd[2])
{
	return trace_rt->pipe(pipefd);
}

int pipe2(int pipefd[2], int flags)
{
	return trace_rt->pipe2(pipefd, flags);
}

/*
 * Initialization functions.
 */
static int init_open(const char* pathname, int flags, ...)
{
	trace_initialize();
	int rc = 0;
	if (flags & O_CREAT) {
		va_list args;
		va_start(args, flags);
		rc = trace_init_rt->open(pathname, flags, va_arg(args, int));
		va_end(args);
	}
	else {
		rc = trace_init_rt->open(pathname, flags);
	}
	return rc;
}

/*
 * Initialization functions.
 */
static int init_open64(const char* pathname, int flags, ...)
{
	trace_initialize();
	int rc = 0;
	if (flags & O_CREAT) {
		va_list args;
		va_start(args, flags);
		rc = trace_init_rt->open64(pathname, flags, va_arg(args, int));
		va_end(args);
	}
	else {
		rc = trace_init_rt->open64(pathname, flags);
	}
	return rc;
}

static int init_close(int fd)
{
	trace_initialize();
	return trace_init_rt->close(fd);
}

static int init_dup2(int oldfd, int newfd)
{
	trace_initialize();
	return trace_init_rt->dup2(oldfd, newfd);
}

static int init_socket(int domain, int type, int protocol)
{
	trace_initialize();
	return trace_init_rt->socket(domain, type, protocol);
}

static FILE *init_fopen(const char *path, const char *mode)
{
	trace_initialize();
	return trace_init_rt->fopen(path, mode);
}

static FILE *init_fdopen(int fd, const char *mode)
{
	trace_initialize();
	return trace_init_rt->fdopen(fd, mode);
}

static FILE *init_freopen(const char *path, const char *mode, FILE *stream)
{
	trace_initialize();
	return trace_init_rt->freopen(path, mode, stream);
}

static int init_fclose(FILE *fp)
{
	trace_initialize();
	return trace_init_rt->fclose(fp);
}

static int init_fcloseall(void)
{
	trace_initialize();
	return trace_init_rt->fcloseall();
}

static int init_creat(const char *pathname, mode_t mode)
{
	trace_initialize();
	return trace_init_rt->creat(pathname, mode);
}

static int init_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	trace_initialize();
	return trace_init_rt->accept(sockfd, addr, addrlen);
}

static int init_dup(int oldfd)
{
	trace_initialize();
	return trace_init_rt->dup(oldfd);
}


static int init_fcntl(int fd, int cmd, ...)
{
	trace_initialize();
	int rc;
	switch (cmd) {
		case F_DUPFD:
		case F_DUPFD_CLOEXEC:
		case F_SETFD:
		case F_SETFL:
		case F_SETLK:
		case F_SETLKW:
		case F_GETLK:
		case F_SETOWN:
		case F_GETOWN_EX:
		case F_SETOWN_EX:
		case F_SETSIG:
		case F_SETLEASE:
		case F_NOTIFY:
//		case F_SETPIPE_SZ:
		{
			va_list args;
			va_start(args, cmd);
			rc = trace_init_rt->fcntl(fd, cmd, va_arg(args, long));
			va_end(args);
			break;
		}

		default: {
			rc = trace_init_rt->fcntl(fd, cmd);
			break;
		}
	}
	return rc;
}

static int init_socketpair(int domain, int type, int protocol, int sv[2])
{
	trace_initialize();
	return trace_init_rt->socketpair(domain, type, protocol, sv);
}

static int init_inotify_init(void)
{
	trace_initialize();
	return trace_init_rt->inotify_init();
}

static int init_pipe(int pipefd[2])
{
	trace_initialize();
	return trace_init_rt->pipe(pipefd);
}

static int init_pipe2(int pipefd[2], int flags)
{
	trace_initialize();
	return trace_init_rt->pipe2(pipefd, flags);
}


static trace_t trace_init = {
	.open = init_open,
	.open64 = init_open64,
	.close = init_close,
	.dup2 = init_dup2,
	.socket = init_socket,
	.fopen = init_fopen,
	.fdopen = init_fdopen,
	.freopen = init_freopen,
	.fclose = init_fclose,
	.fcloseall = init_fcloseall,
	.creat = init_creat,
	.accept = init_accept,
	.dup = init_dup,
	.fcntl = init_fcntl,
	.socketpair = init_socketpair,
	.inotify_init = init_inotify_init,
	.pipe = init_pipe,
	.pipe2 = init_pipe2,
};

/* */

/*
 * Library initialization/deinitialization
 */

static void trace_file_init(void) __attribute__((constructor));
static void trace_file_fini(void) __attribute__((destructor));

static void trace_file_init(void)
{
	trace_initialize();
}


static void trace_file_fini(void)
{
	enable_tracing(false);
	LOG("fini");
}

/**
 * Gets module information data.
 *
 * @return  the module information data.
 */
const sp_rtrace_module_info_t* sp_rtrace_get_module_info(void)
{
	return &module_info;
}


