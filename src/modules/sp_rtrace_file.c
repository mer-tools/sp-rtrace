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
#include <dlfcn.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>

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

/* resource identifiers */
static int res_fp = 0;
static int res_fd = 0;


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


typedef struct {
	fopen_t fopen;
	fdopen_t fdopen;
	freopen_t freopen;
	fclose_t fclose;
	fcloseall_t fcloseall;
	open_t open;
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
static void trace_initialize()
{
	static int init_mode = MODULE_UNINITIALIZED;
	switch (init_mode) {
		case MODULE_UNINITIALIZED: {
			trace_off.open = (open_t)dlsym(RTLD_NEXT, "open");
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
			init_mode = MODULE_LOADED;

			LOG("module loaded: %s (%d.%d)", module_info.name, module_info.version_major, module_info.version_minor);
		}

		case MODULE_LOADED: {
			if (sp_rtrace_initialize()) {
				sp_rtrace_register_module(module_info.name, module_info.version_major, module_info.version_minor, enable_tracing);
				res_fd = sp_rtrace_register_resource("fd", "file descriptor");
				res_fp = sp_rtrace_register_resource("fp", "file pointer");
				trace_init_rt = trace_rt;
				init_mode = MODULE_READY;

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
		char arg2[16];
		const char *args[] = {pathname, arg2, NULL};
		sprintf(arg2, "%x", flags);
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fd, "open", 1, (pointer_t)rc, args);
	}
	return rc;
}


static int trace_close(int fd)
{
	int rc = trace_off.close(fd);
	backtrace_lock = 0;
	if (rc != -1) {
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_FREE, res_fd, "close", 0, (pointer_t)fd, NULL);
	}
	return rc;
}

static int trace_dup2(int oldfd, int newfd)
{
	int rc = trace_off.dup2(oldfd, newfd);
	if (rc != -1) {
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_FREE, res_fd, "dup2", 0, (pointer_t)newfd, NULL);
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fd, "dup2", 1, (pointer_t)rc, NULL);
	}
	return rc;
}

static int trace_socket(int domain, int type, int protocol)
{
	int rc = trace_off.socket(domain, type, protocol);
	if (rc != -1) {
		char arg1[64], arg2[64], arg3[64];
		const char* args[] = {arg1, arg2, arg3, NULL};
		sprintf(arg1, "domain:0x%x", domain);
		sprintf(arg2, "type:0x%x", type);
		sprintf(arg3, "protocol:0x%x", protocol);
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fd, "socket", 1, (pointer_t)rc, args);
	}
	return rc;
}

static FILE *trace_fopen(const char *path, const char *mode)
{
	FILE* rc = trace_off.fopen(path, mode);
	if (rc) {
		const char* args[] = {path, mode, NULL};
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fp, "fopen", 1, (pointer_t)rc, args);
	}
	return rc;
}

static FILE *trace_fdopen(int fd, const char *mode)
{
	FILE* rc = trace_off.fdopen(fd, mode);
	if (rc) {
		char arg1[32];
		const char* args[] = {arg1, mode, NULL};
		sprintf(arg1, "fd:0x%x", fd);
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_FREE, res_fd, "fdopen", 0, (pointer_t)fd, NULL);
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fp, "fdopen", 1, (pointer_t)rc, args);
	}
	return rc;
}

static FILE *trace_freopen(const char *path, const char *mode, FILE *stream)
{
	FILE* rc = trace_off.freopen(path, mode, stream);
	if (rc) {
		const char* args[] = {path, mode, NULL};
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_FREE, res_fp, "freopen", 0, (pointer_t)stream, NULL);
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fp, "freopen", 1, (pointer_t)rc, args);
	}
	return rc;
}

static int trace_fclose(FILE *fp)
{
	int rc = trace_off.fclose(fp);
	if (rc == 0) {
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_FREE, res_fp, "fclose", 0, (pointer_t)fp, NULL);
	}
	return rc;
}

static int trace_fcloseall()
{
	int rc = trace_off.fcloseall();
	if (rc == 0) {
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_FREE, res_fp, "fcloseall", 0, (pointer_t)-1, NULL);
	}
	return rc;
}

static int trace_creat(const char *pathname, mode_t mode)
{
	int rc = trace_off.creat(pathname, mode);
	if (rc != -1) {
		char arg2[32];
		const char* args[] = {pathname, arg2, NULL};
		sprintf(arg2, "mode:0x%x", mode);
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fd, "creat", 1, (pointer_t)rc, args);
	}
	return rc;
}

static int trace_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int rc = trace_off.accept(sockfd, addr, addrlen);
	if (rc != -1) {
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fd, "accept", 1, (pointer_t)rc, NULL);
	}
	return rc;
}

static int trace_dup(int oldfd)
{
	int rc = trace_off.dup(oldfd);
	if (rc != -1) {
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fd, "dup", 1, (pointer_t)rc, NULL);
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
				sp_rtrace_write_function_call(SP_RTRACE_FTYPE_FREE, res_fd, "fcntl", 1, (pointer_t)newfd, NULL);
				sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fd, "fcntl", 1, (pointer_t)rc, NULL);
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
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fd, "socketpair", 1, (pointer_t)sv[0], NULL);
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fd, "socketpair", 1, (pointer_t)sv[1], NULL);
	}
	return rc;
}

static int trace_inotify_init(void)
{
	int rc = trace_off.inotify_init();
	if (rc != -1) {
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fd, "inotify_init", 1, (pointer_t)rc, NULL);
	}
	return rc;
}

static int trace_pipe(int pipefd[2])
{
	int rc = trace_off.pipe(pipefd);
	if (rc != -1) {
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fd, "pipe", 1, (pointer_t)pipefd[0], NULL);
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fd, "pipe", 1, (pointer_t)pipefd[1], NULL);
	}
	return rc;
}


static trace_t trace_on = {
	.open = trace_open,
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
		BT_RETURN_IF_LOCKED(trace_off.open(pathname, flags, mode));
		BT_LOCK_AND_EXECUTE(rc = trace_rt->open(pathname, flags, va_arg(args, int)));
	}
	else {
		BT_RETURN_IF_LOCKED(trace_off.open(pathname, flags));
		BT_LOCK_AND_EXECUTE(rc = trace_rt->open(pathname, flags));
	}
	return rc;
}

int close(int fd)
{
	/* synchronize allocation functions used by backtrace */
	BT_RETURN_IF_LOCKED(trace_off.close(fd));
	BT_LOCK_AND_EXECUTE(int rc = trace_rt->close(fd));
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

int fcloseall()
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

static int init_fcloseall()
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


static trace_t trace_init = {
	.open = init_open,
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
const sp_rtrace_module_info_t* sp_rtrace_get_module_info()
{
	return &module_info;
}


