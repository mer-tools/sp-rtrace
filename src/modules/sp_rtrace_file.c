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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/inotify.h>
#include <sys/eventfd.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <unistd.h>

#include "sp_rtrace_main.h"
#include "sp_rtrace_module.h"

#include "common/sp_rtrace_proto.h"


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

typedef int (*creat_t)(const char *pathname, mode_t mode);
typedef int (*open_t)(const char* pathname, int flags, ...);
typedef int (*open64_t)(const char* pathname, int flags, ...);
typedef int (*openat_t)(int dirfd, const char* pathname, int flags, ...);
typedef int (*close_t)(int fd);
typedef int (*dup_t)(int oldfd);
typedef int (*dup2_t)(int oldfd, int newfd);
typedef int (*dup3_t)(int oldfd, int newfd, int flags);
typedef int (*fcntl_t)(int fd, int cmd, ...);
typedef int (*socket_t)(int domain, int type, int protocol);
typedef int (*socketpair_t)(int domain, int type, int protocol, int sv[2]);
typedef int (*accept_t)(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
typedef int (*accept4_t)(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags);
typedef int (*inotify_init_t)(void);
typedef int (*inotify_init1_t)(int flags);
typedef int (*rt_eventfd_t)(int initval, int flags);
typedef int (*signalfd_t)(int fd, const sigset_t *mask, int flags);
typedef int (*timerfd_create_t)(int clockid, int flags);
typedef int (*epoll_create_t)(int size);
typedef int (*epoll_create1_t)(int flags);
typedef int (*getpt_t)(void);
typedef int (*posix_openpt_t)(int flags);
typedef int (*pipe_t)(int pipefd[2]);
typedef int (*pipe2_t)(int pipefd[2], int flags);
typedef FILE* (*fopen_t)(const char *path, const char *mode);
typedef FILE* (*fdopen_t)(int fd, const char *mode);
typedef FILE* (*freopen_t)(const char *path, const char *mode, FILE *stream);
typedef int (*fclose_t)(FILE *fp);
typedef int (*fcloseall_t)(void);


typedef struct {
	creat_t creat;
	open_t open;
	open64_t open64;
	openat_t openat;
	close_t close;
	dup_t dup;
	dup2_t dup2;
	dup3_t dup3;
	fcntl_t fcntl;
	socket_t socket;
	socketpair_t socketpair;
	accept_t accept;
	accept4_t accept4;
	inotify_init_t inotify_init;
	inotify_init1_t inotify_init1;
	rt_eventfd_t eventfd;
	signalfd_t signalfd;
	timerfd_create_t timerfd_create;
	epoll_create_t epoll_create;
	epoll_create1_t epoll_create1;
	getpt_t getpt;
	posix_openpt_t posix_openpt;
	pipe_t pipe;
	pipe2_t pipe2;
	fopen_t fopen;
	fdopen_t fdopen;
	freopen_t freopen;
	fclose_t fclose;
	fcloseall_t fcloseall;
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


/* Module information */
static const sp_rtrace_module_info_t module_info = {
	.type = MODULE_TYPE_PRELOAD,
	.version_major = 1,
	.version_minor = 0,
	.symcount = sizeof(trace_t)/sizeof(pointer_t),
	.symtable = (const pointer_t*)&trace_off,
	.name = "file",
	.description = "File operation tracking module. "
		       "Tracks file opening/closing operations.",
};

static module_resource_t res_fd = {
	.type = "fd",
	.desc = "file descriptor",
	.flags = SP_RTRACE_RESOURCE_DEFAULT,
};

static module_resource_t res_fp = {
	.type = "fp",
	.desc = "file pointer",
	.flags = SP_RTRACE_RESOURCE_DEFAULT,
};


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
			trace_off.creat = (creat_t)dlsym(RTLD_NEXT, "creat");
			trace_off.open = (open_t)dlsym(RTLD_NEXT, "open");
			trace_off.open64 = (open_t)dlsym(RTLD_NEXT, "open64");
			trace_off.openat = (openat_t)dlsym(RTLD_NEXT, "openat");
			trace_off.close = (close_t)dlsym(RTLD_NEXT, "close");
			trace_off.dup = (dup_t)dlsym(RTLD_NEXT, "dup");
			trace_off.dup2 = (dup2_t)dlsym(RTLD_NEXT, "dup2");
			trace_off.dup3 = (dup3_t)dlsym(RTLD_NEXT, "dup3");
			trace_off.fcntl = (fcntl_t)dlsym(RTLD_NEXT, "fcntl");
			trace_off.socket = (socket_t)dlsym(RTLD_NEXT, "socket");
			trace_off.socketpair = (socketpair_t)dlsym(RTLD_NEXT, "socketpair");
			trace_off.accept = (accept_t)dlsym(RTLD_NEXT, "accept");
			trace_off.accept4 = (accept4_t)dlsym(RTLD_NEXT, "accept4");
			trace_off.inotify_init = (inotify_init_t)dlsym(RTLD_NEXT, "inotify_init");
			trace_off.inotify_init1 = (inotify_init1_t)dlsym(RTLD_NEXT, "inotify_init1");
			trace_off.eventfd = (rt_eventfd_t)dlsym(RTLD_NEXT, "eventfd");
			trace_off.signalfd = (signalfd_t)dlsym(RTLD_NEXT, "signalfd");
			trace_off.timerfd_create = (timerfd_create_t)dlsym(RTLD_NEXT, "timerfd_create");
			trace_off.epoll_create = (epoll_create_t)dlsym(RTLD_NEXT, "epoll_create");
			trace_off.epoll_create1 = (epoll_create1_t)dlsym(RTLD_NEXT, "epoll_create1");
			trace_off.getpt = (getpt_t)dlsym(RTLD_NEXT, "getpt");
			trace_off.posix_openpt = (posix_openpt_t)dlsym(RTLD_NEXT, "posix_openpt");
			trace_off.pipe = (pipe_t)dlsym(RTLD_NEXT, "pipe");
			trace_off.pipe2 = (pipe2_t)dlsym(RTLD_NEXT, "pipe2");
			trace_off.fopen = (fopen_t)dlsym(RTLD_NEXT, "fopen");
			trace_off.fdopen = (fdopen_t)dlsym(RTLD_NEXT, "fdopen");
			trace_off.freopen = (freopen_t)dlsym(RTLD_NEXT, "freopen");
			trace_off.fclose = (fclose_t)dlsym(RTLD_NEXT, "fclose");
			trace_off.fcloseall = (fcloseall_t)dlsym(RTLD_NEXT, "fcloseall");
			init_mode = MODULE_LOADED;

			LOG("module loaded: %s (%d.%d)", module_info.name, module_info.version_major, module_info.version_minor);
		}

		case MODULE_LOADED: {
			if (sp_rtrace_initialize() && init_mode == MODULE_LOADED) {
				init_mode = MODULE_READY;

				sp_rtrace_register_module(&module_info, enable_tracing);
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

static int trace_creat(const char *pathname, mode_t mode)
{
	int rc = trace_off.creat(pathname, mode);
	if (rc != -1) {
		char mode_s[16];
		sprintf(mode_s, "0x%x", mode);

		module_fcall_t call = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type_id = res_fd.id,
				.name = "creat",
				.res_size = 1,
				.res_id = (pointer_t)rc,
		};
		module_farg_t args[] = {
				{.name = "path", .value = pathname},
				{.name = "mode", .value = mode_s},
				{.name = NULL, .value = NULL}
		};
		sp_rtrace_write_function_call(&call, NULL, args);
	}
	return rc;
}

/* code common to all open*() traces */
static void trace_open_common(const char* name, int fd, const char* path, int flags)
{
	char flags_s[16];
	sprintf(flags_s, "0x%x", flags);

	module_fcall_t call = {
		.type = SP_RTRACE_FTYPE_ALLOC,
		.res_type_id = res_fd.id,
		.name = name,
		.res_size = 1,
		.res_id = (pointer_t)fd,
	};
	module_farg_t args[] = {
		{.name = "path", .value = path},
		{.name = "flags", .value = flags_s},
		{.name = NULL, .value = NULL}
	};
	sp_rtrace_write_function_call(&call, NULL, args);
}

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
		trace_open_common("open", rc, pathname, flags);
	}
	return rc;
}

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
		trace_open_common("open64", rc, pathname, flags);
	}
	return rc;
}

static int trace_openat(int dirfd, const char* pathname, int flags, ...)
{
	int rc = 0;
	if (flags & O_CREAT) {
		va_list args;
		va_start(args, flags);
		rc = trace_off.openat(dirfd, pathname, flags, va_arg(args, int));
		va_end(args);
	}
	else {
		rc = trace_off.openat(dirfd, pathname, flags);
	}
	if (rc != -1) {
		trace_open_common("openat", rc, pathname, flags);
	}
	return rc;
}

static int trace_close(int fd)
{
	int rc = trace_off.close(fd);
	backtrace_lock = 0;
	if (rc != -1) {
		module_fcall_t call = {
				.type = SP_RTRACE_FTYPE_FREE,
				.res_type_id = res_fd.id,
				.name = "close",
				.res_size = 0,
				.res_id = (pointer_t)fd,
		};
		sp_rtrace_write_function_call(&call, NULL, NULL);
	}
	return rc;
}

/* code common to all dup*() traces */
static void trace_dup_common(const char* name, int oldfd, int newfd, int retfd)
{
	char oldfd_s[16]; 
	sprintf(oldfd_s, "%d", oldfd);
	module_farg_t args[] = {
		{.name = "oldfd", .value = oldfd_s},
		{.name = NULL, .value = NULL}
	};
	if (newfd >= 0) {
		/* newfd is first closed if it was already open */
		module_fcall_t call1 = {
			.type = SP_RTRACE_FTYPE_FREE,
			.res_type_id = res_fd.id,
			.name = name,
			.res_size = 0,
			.res_id = (pointer_t)newfd,
		};
		sp_rtrace_write_function_call(&call1, NULL, args);
	}
	module_fcall_t call2 = {
		.type = SP_RTRACE_FTYPE_ALLOC,
		.res_type_id = res_fd.id,
		.name = name,
		.res_size = 1,
		.res_id = (pointer_t)retfd,
	};
	sp_rtrace_write_function_call(&call2, NULL, args);
}

static int trace_dup(int oldfd)
{
	int rc = trace_off.dup(oldfd);
	if (rc != -1) {
		trace_dup_common("dup", oldfd, -1, rc);
	}
	return rc;
}

static int trace_dup2(int oldfd, int newfd)
{
	int rc = trace_off.dup2(oldfd, newfd);
	if (rc != -1 && oldfd != newfd) {
		trace_dup_common("dup2", oldfd, newfd, rc);
	}
	return rc;
}

static int trace_dup3(int oldfd, int newfd, int flags)
{
	int rc = trace_off.dup3(oldfd, newfd, flags);
	if (rc != -1 && oldfd != newfd) {
		trace_dup_common("dup3", oldfd, newfd, rc);
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
			long newfd = va_arg(args, long);
			va_end(args);
			rc = trace_off.fcntl(fd, cmd, newfd);
			if ( (cmd == F_DUPFD || cmd == F_DUPFD_CLOEXEC) && rc != -1) {
				trace_dup_common("fcntl", fd, newfd, rc);
			}
			break;
		}

		default: {
			/* rest of fcntl operations recognized by the trap fcntl() function */
			rc = trace_off.fcntl(fd, cmd);
			break;
		}
	}

	return rc;
}

/* code common to socket*() functions */
static void trace_socket_common(const char* name, int fd, int domain, int type, int protocol)
{
	char domain_s[64], type_s[64], protocol_s[64];
	snprintf(domain_s, sizeof(domain_s), "0x%x", domain);
	snprintf(type_s, sizeof(type_s), "0x%x", type);
	snprintf(protocol_s, sizeof(protocol_s), "0x%x", protocol);

	module_fcall_t call = {
		.type = SP_RTRACE_FTYPE_ALLOC,
		.res_type_id = res_fd.id,
		.name = name,
		.res_size = 1,
		.res_id = (pointer_t)fd,
	};
	module_farg_t args[] = {
		{.name = "domain", .value = domain_s},
		{.name = "type", .value = type_s},
		{.name = "protocol", .value = protocol_s},
		{.name = NULL, .value = NULL}
	};
	sp_rtrace_write_function_call(&call, NULL, args);
}

static int trace_socket(int domain, int type, int protocol)
{
	int rc = trace_off.socket(domain, type, protocol);
	if (rc != -1) {
		trace_socket_common("socket", rc, domain, type, protocol);
	}
	return rc;
}

static int trace_socketpair(int domain, int type, int protocol, int sv[2])
{
	int rc = trace_off.socketpair(domain, type, protocol, sv);
	if (rc != -1) {
		trace_socket_common("socketpair", sv[0], domain, type, protocol);
		trace_socket_common("socketpair", sv[1], domain, type, protocol);
	}
	return rc;
}

/* code common to both accept*() traces */
static void trace_accept_common(int fd, const char *name, struct sockaddr *addr)
{
	module_farg_t args[] = {
		{.name = NULL, .value = NULL},
		{.name = NULL, .value = NULL}
	};
	/* unix socket address with valid nil-terminated name string?
	 * (use sun_path as it's larger than BSD sa_data)
	 */
	if (addr && addr->sa_family == AF_UNIX &&
	    memchr(addr->sa_data, '\0', sizeof(((struct sockaddr_un*)addr)->sun_path))) {
		args[0].name = "path";
		args[0].value = ((struct sockaddr_un*)addr)->sun_path;
	}
	module_fcall_t call = {
		.type = SP_RTRACE_FTYPE_ALLOC,
		.res_type_id = res_fd.id,
		.name = name,
		.res_size = 1,
		.res_id = (pointer_t)fd,
	};
	sp_rtrace_write_function_call(&call, NULL, args);
}

static int trace_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int rc = trace_off.accept(sockfd, addr, addrlen);
	if (rc != -1) {
		trace_accept_common(rc, "accept", addr);
	}
	return rc;
}

static int trace_accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags)
{
	int rc = trace_off.accept4(sockfd, addr, addrlen, flags);
	if (rc != -1) {
		trace_accept_common(rc, "accept4", addr);
	}
	return rc;
}

/* code common to misc *fd*() traces */
static void trace_fd_common(const char *name, int fd)
{
	module_fcall_t call = {
		.type = SP_RTRACE_FTYPE_ALLOC,
		.res_type_id = res_fd.id,
		.name = name,
		.res_size = 1,
		.res_id = (pointer_t)fd,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
}

static int trace_inotify_init(void)
{
	int rc = trace_off.inotify_init();
	if (rc != -1) {
		trace_fd_common("inotify_init", rc);
	}
	return rc;
}

static int trace_inotify_init1(int flags)
{
	int rc = trace_off.inotify_init1(flags);
	if (rc != -1) {
		trace_fd_common("inotify_init1", rc);
	}
	return rc;
}

static int trace_eventfd(int initval, int flags)
{
	int rc = trace_off.eventfd(initval, flags);
	if (rc != -1) {
		trace_fd_common("eventfd", rc);
	}
	return rc;
}

static int trace_signalfd(int fd, const sigset_t *mask, int flags)
{
	int rc = trace_off.signalfd(fd, mask, flags);
	if (fd == -1 && rc != -1) {
		trace_fd_common("signalfd", rc);
	}
	return rc;
}

static int trace_timerfd_create(int clockid, int flags)
{
	int rc = trace_off.timerfd_create(clockid, flags);
	if (rc != -1) {
		trace_fd_common("timerfd_create", rc);
	}
	return rc;
}

static int trace_epoll_create(int size)
{
	int rc = trace_off.epoll_create(size);
	if (rc != -1) {
		trace_fd_common("epoll_create", rc);
	}
	return rc;
}

static int trace_epoll_create1(int flags)
{
	int rc = trace_off.epoll_create1(flags);
	if (rc != -1) {
		trace_fd_common("epoll_create1", rc);
	}
	return rc;
}

static int trace_getpt(void)
{
	int rc = trace_off.getpt();
	if (rc != -1) {
		trace_fd_common("getpt", rc);
	}
	return rc;
}

static int trace_posix_openpt(int flags)
{
	int rc = trace_off.posix_openpt(flags);
	if (rc != -1) {
		trace_fd_common("posix_openpt", rc);
	}
	return rc;
}

static int trace_pipe(int pipefd[2])
{
	int rc = trace_off.pipe(pipefd);
	if (rc != -1) {
		trace_fd_common("pipe", pipefd[0]);
		trace_fd_common("pipe", pipefd[1]);
	}
	return rc;
}

static int trace_pipe2(int pipefd[2], int flags)
{
	int rc = trace_off.pipe2(pipefd, flags);
	if (rc != -1) {
		char flags_s[16];
		snprintf(flags_s, sizeof(flags_s), "0x%x", flags);
		module_farg_t args[] = {
				{.name = "flags", .value = flags_s},
				{.name = NULL, .value = NULL}
		};
		module_fcall_t call = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type_id = res_fd.id,
				.name = "pipe2",
				.res_size = 1,
				.res_id = (pointer_t)pipefd[0],
		};
		sp_rtrace_write_function_call(&call, NULL, args);

		call.res_id = (pointer_t)pipefd[1];
		sp_rtrace_write_function_call(&call, NULL, args);
	}
	return rc;
}

/* code common to misc FILE* opening functions */
static void trace_fopen_common(const char *name, FILE *fp, const char *path, const char *mode)
{
	module_fcall_t call = {
		.type = SP_RTRACE_FTYPE_ALLOC,
		.res_type_id = res_fp.id,
		.name = name,
		.res_size = 1,
		.res_id = (pointer_t)fp,
	};
	module_farg_t args[] = {
		{.name = "path", .value = path},
		{.name = "mode", .value = mode},
		{.name = NULL, .value = NULL}
	};
	sp_rtrace_write_function_call(&call, NULL, args);
}

static FILE *trace_fopen(const char *path, const char *mode)
{
	FILE* rc = trace_off.fopen(path, mode);
	if (rc) {
		trace_fopen_common("fopen", rc, path, mode);
	}
	return rc;
}

static FILE *trace_fdopen(int fd, const char *mode)
{
	FILE* rc = trace_off.fdopen(fd, mode);
	if (rc) {
		char fd_s[32];
		sprintf(fd_s, "%d", fd);
		/* The file  descriptor is not dup'ed, but it will be
		 * closed when the stream created by fdopen() is closed.
		 */
		module_fcall_t call1 = {
				.type = SP_RTRACE_FTYPE_FREE,
				.res_type_id = res_fd.id,
				.name = "fdopen",
				.res_size = 0,
				.res_id = (pointer_t)fd,
		};
		sp_rtrace_write_function_call(&call1, NULL, NULL);

		module_fcall_t call2 = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type_id = res_fp.id,
				.name = "fdopen",
				.res_size = 1,
				.res_id = (pointer_t)rc,
		};
		module_farg_t args[] = {
				{.name = "fd", .value = fd_s},
				{.name = "mode", .value = mode},
				{.name = NULL, .value = NULL}
		};
		sp_rtrace_write_function_call(&call2, NULL, args);

	}
	return rc;
}

/* code common to misc FILE* closing functions */
static void trace_fclose_common(const char *name, FILE *fp)
{
	module_fcall_t call = {
		.type = SP_RTRACE_FTYPE_FREE,
		.res_type_id = res_fp.id,
		.name = name,
		.res_size = 0,
		.res_id = (pointer_t)fp,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
}

static FILE *trace_freopen(const char *path, const char *mode, FILE *stream)
{
	FILE* rc = trace_off.freopen(path, mode, stream);
	if (rc) {
		trace_fclose_common("freopen", stream);
		trace_fopen_common("freopen", rc, path, mode);
	}
	return rc;
}

static int trace_fclose(FILE *fp)
{
	int rc = trace_off.fclose(fp);
	if (rc == 0) {
		trace_fclose_common("fclose", fp);
	}
	return rc;
}

static int trace_fcloseall(void)
{
	int rc = trace_off.fcloseall();
	if (rc == 0) {
		trace_fclose_common("fcloseall", (FILE *)-1);
	}
	return rc;
}


static trace_t trace_on = {
	.creat = trace_creat,
	.open = trace_open,
	.open64 = trace_open64,
	.openat = trace_openat,
	.close = trace_close,
	.dup = trace_dup,
	.dup2 = trace_dup2,
	.dup3 = trace_dup3,
	.fcntl = trace_fcntl,
	.socket = trace_socket,
	.socketpair = trace_socketpair,
	.accept = trace_accept,
	.accept4 = trace_accept4,
	.inotify_init = trace_inotify_init,
	.inotify_init1 = trace_inotify_init1,
	.eventfd = trace_eventfd,
	.signalfd = trace_signalfd,
	.timerfd_create = trace_timerfd_create,
	.epoll_create = trace_epoll_create,
	.epoll_create1 = trace_epoll_create1,
	.getpt = trace_getpt,
	.posix_openpt = trace_posix_openpt,
	.pipe = trace_pipe,
	.pipe2 = trace_pipe2,
	.fopen = trace_fopen,
	.fdopen = trace_fdopen,
	.freopen = trace_freopen,
	.fclose = trace_fclose,
	.fcloseall = trace_fcloseall,
};


/* target functions */

int creat(const char *pathname, mode_t mode)
{
	return trace_rt->creat(pathname, mode);
}

int open(const char* pathname, int flags, ...)
{
	int rc;
	if (flags & O_CREAT) {
		int mode;
		va_list args;
		va_start(args, flags);
		mode = va_arg(args, int);
		va_end(args);
		BT_EXECUTE_LOCKED(rc = trace_rt->open(pathname, flags, mode), trace_off.open(pathname, flags, mode));
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
		BT_EXECUTE_LOCKED(rc = trace_rt->open64(pathname, flags, mode), trace_off.open64(pathname, flags, mode));
	}
	else {
		BT_EXECUTE_LOCKED(rc = trace_rt->open64(pathname, flags), trace_off.open64(pathname, flags));
	}
	return rc;
}

int openat(int dirfd, const char* pathname, int flags, ...)
{
	int rc;
	if (flags & O_CREAT) {
		va_list args;
		va_start(args, flags);
		rc = trace_rt->openat(dirfd, pathname, flags, va_arg(args, int));
		va_end(args);
	}
	else {
		rc = trace_rt->openat(dirfd, pathname, flags);
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

int dup(int oldfd)
{
	return trace_rt->dup(oldfd);
}

int dup2(int oldfd, int newfd)
{
	return trace_rt->dup2(oldfd, newfd);
}

int dup3(int oldfd, int newfd, int flags)
{
	return trace_rt->dup3(oldfd, newfd, flags);
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
			fprintf(stderr, "ERROR: unknown sp-rtrace file module fcntl command: %d (0x%x)\n", cmd, cmd);
			exit (-1);
		}
	}
	return rc;
}

int socket(int domain, int type, int protocol)
{
	return trace_rt->socket(domain, type, protocol);
}

int socketpair(int domain, int type, int protocol, int sv[2])
{
	return trace_rt->socketpair(domain, type, protocol, sv);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	return trace_rt->accept(sockfd, addr, addrlen);
}

int accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags)
{
	return trace_rt->accept4(sockfd, addr, addrlen, flags);
}

int inotify_init(void)
{
	return trace_rt->inotify_init();
}

int inotify_init1(int flags)
{
	return trace_rt->inotify_init1(flags);
}

int eventfd(int initval, int flags)
{
	return trace_rt->eventfd(initval, flags);
}

int signalfd(int fd, const sigset_t *mask, int flags)
{
	return trace_rt->signalfd(fd, mask, flags);
}

int timerfd_create(int clockid, int flags)
{
	return trace_rt->timerfd_create(clockid, flags);
}

int epoll_create(int size)
{
	return trace_rt->epoll_create(size);
}

int epoll_create1(int flags)
{
	return trace_rt->epoll_create1(flags);
}

int getpt(void)
{
	return trace_rt->getpt();
}

int posix_openpt(int flags)
{
	return trace_rt->posix_openpt(flags);
}

int pipe(int pipefd[2])
{
	return trace_rt->pipe(pipefd);
}

int pipe2(int pipefd[2], int flags)
{
	return trace_rt->pipe2(pipefd, flags);
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

/*
 * Initialization functions.
 */

static int init_creat(const char *pathname, mode_t mode)
{
	trace_initialize();
	return trace_init_rt->creat(pathname, mode);
}

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

static int init_openat(int dirfd, const char* pathname, int flags, ...)
{
	trace_initialize();
	int rc = 0;
	if (flags & O_CREAT) {
		va_list args;
		va_start(args, flags);
		rc = trace_init_rt->openat(dirfd, pathname, flags, va_arg(args, int));
		va_end(args);
	}
	else {
		rc = trace_init_rt->openat(dirfd, pathname, flags);
	}
	return rc;
}

static int init_close(int fd)
{
	trace_initialize();
	return trace_init_rt->close(fd);
}

static int init_dup(int oldfd)
{
	trace_initialize();
	return trace_init_rt->dup(oldfd);
}

static int init_dup2(int oldfd, int newfd)
{
	trace_initialize();
	return trace_init_rt->dup2(oldfd, newfd);
}

static int init_dup3(int oldfd, int newfd, int flags)
{
	trace_initialize();
	return trace_init_rt->dup3(oldfd, newfd, flags);
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

static int init_socket(int domain, int type, int protocol)
{
	trace_initialize();
	return trace_init_rt->socket(domain, type, protocol);
}

static int init_socketpair(int domain, int type, int protocol, int sv[2])
{
	trace_initialize();
	return trace_init_rt->socketpair(domain, type, protocol, sv);
}

static int init_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	trace_initialize();
	return trace_init_rt->accept(sockfd, addr, addrlen);
}

static int init_accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags)
{
	trace_initialize();
	return trace_init_rt->accept4(sockfd, addr, addrlen, flags);
}

static int init_inotify_init(void)
{
	trace_initialize();
	return trace_init_rt->inotify_init();
}

static int init_inotify_init1(int flags)
{
	trace_initialize();
	return trace_init_rt->inotify_init1(flags);
}

static int init_eventfd(int initval, int flags)
{
	trace_initialize();
	return trace_init_rt->eventfd(initval, flags);
}

static int init_signalfd(int fd, const sigset_t *mask, int flags)
{
	trace_initialize();
	return trace_init_rt->signalfd(fd, mask, flags);
}

static int init_timerfd_create(int clockid, int flags)
{
	trace_initialize();
	return trace_init_rt->timerfd_create(clockid, flags);
}

static int init_epoll_create(int size)
{
	trace_initialize();
	return trace_init_rt->epoll_create(size);
}

static int init_epoll_create1(int flags)
{
	trace_initialize();
	return trace_init_rt->epoll_create1(flags);
}

static int init_getpt(void)
{
	trace_initialize();
	return trace_init_rt->getpt();
}

static int init_posix_openpt(int flags)
{
	trace_initialize();
	return trace_init_rt->posix_openpt(flags);
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


static trace_t trace_init = {
	.creat = init_creat,
	.open = init_open,
	.open64 = init_open64,
	.openat = init_openat,
	.close = init_close,
	.dup = init_dup,
	.dup2 = init_dup2,
	.dup3 = init_dup3,
	.fcntl = init_fcntl,
	.socket = init_socket,
	.socketpair = init_socketpair,
	.accept = init_accept,
	.accept4 = init_accept4,
	.inotify_init = init_inotify_init,
	.inotify_init1 = init_inotify_init1,
	.eventfd = init_eventfd,
	.signalfd = init_signalfd,
	.timerfd_create = init_timerfd_create,
	.epoll_create = init_epoll_create,
	.epoll_create1 = init_epoll_create1,
	.getpt = init_getpt,
	.posix_openpt = init_posix_openpt,
	.pipe = init_pipe,
	.pipe2 = init_pipe2,
	.fopen = init_fopen,
	.fdopen = init_fdopen,
	.freopen = init_freopen,
	.fclose = init_fclose,
	.fcloseall = init_fcloseall,
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
