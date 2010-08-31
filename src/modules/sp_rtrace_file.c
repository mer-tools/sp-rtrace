
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
#include <dlfcn.h>
#include <stdarg.h>

#include "sp_rtrace_main.h"
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

 /*
  * file module function set
  */

typedef int (*open_t)(const char* pathname, int flags, ...);
typedef int (*close_t)(int fd);
typedef int (*dup2_t)(int oldfd, int newfd);
typedef int (*socket_t)(int domain, int type, int protocol);


typedef struct {
	open_t open;
	close_t close;
	dup2_t dup2;
	socket_t socket;
} trace_file_t;

/* original function references */
static trace_file_t trace_off;
/* tracing function references */
static trace_file_t trace_on;
/* tracing function initializers */
static trace_file_t trace_init;

/* Runtime function references */
static trace_file_t* trace_rt = &trace_init;


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
	static bool is_initialized = false;
	if (!is_initialized) {
		LOG("initializing %s (%d.%d)", module_info.name, module_info.version_major, module_info.version_minor);

		trace_off.open = (open_t)dlsym(RTLD_NEXT, "open");
		trace_off.close = (close_t)dlsym(RTLD_NEXT, "close");
		trace_off.dup2 = (dup2_t)dlsym(RTLD_NEXT, "dup2");
		trace_off.socket = (socket_t)dlsym(RTLD_NEXT, "socket");

		enable_tracing(false);

		sp_rtrace_initialize();

		sp_rtrace_register_module(module_info.name, module_info.version_major, module_info.version_minor, enable_tracing);
		res_fd = sp_rtrace_register_resource("fd", "file descriptor");
		res_fp = sp_rtrace_register_resource("fp", "file pointer");

		is_initialized = true;
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
	if (rc != -1) {
		char arg2[16];
		const char *args[] = {pathname, arg2, NULL};
		sprintf(arg2, "%x", flags);
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fd, "open", 1, (void*)(long)rc, args);
	}
	return rc;
}


static int trace_close(int fd)
{
	int rc = trace_off.close(fd);
	if (rc != -1) {
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_FREE, res_fd, "close", 0, (void*)(long)fd, NULL);
	}
	return rc;
}

static int trace_dup2(int oldfd, int newfd)
{
	int rc = trace_off.dup2(oldfd, newfd);
	if (rc != -1) {
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_FREE, res_fd, "dup2", 0, (void*)(long)newfd, NULL);
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fd, "dup2", 1, (void*)(long)rc, NULL);
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
		sp_rtrace_write_function_call(SP_RTRACE_FTYPE_ALLOC, res_fd, "socket", 1, (void*)(long)rc, args);
	}
	return rc;
}


static trace_file_t trace_on = {
	.open = trace_open,
	.close = trace_close,
	.dup2 = trace_dup2,
	.socket = trace_socket,
};


/* target functions */
int open(const char* pathname, int flags, ...)
{
	int rc;
	if (flags & O_CREAT) {
		va_list args;
		va_start(args, flags);
		rc = trace_rt->open(pathname, flags, va_arg(args, int));
		va_end(args);
	}
	else {
		rc = trace_rt->open(pathname, flags);
	}
	return rc;
}


int close(int fd)
{
	return trace_rt->close(fd);
}

int dup2(int oldfd, int newfd)
{
	return trace_rt->dup2(oldfd, newfd);
}

int socket(int domain, int type, int protocol)
{
	return trace_rt->socket(domain, type, protocol);
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
		rc = trace_rt->open(pathname, flags, va_arg(args, int));
		va_end(args);
	}
	else {
		rc = trace_rt->open(pathname, flags);
	}
	return rc;
}


static int init_close(int fd)
{
	trace_initialize();
	return trace_rt->close(fd);
}

static int init_dup2(int oldfd, int newfd)
{
	trace_initialize();
	return trace_rt->dup2(oldfd, newfd);
}

static int init_socket(int domain, int type, int protocol)
{
	trace_initialize();
	return trace_rt->socket(domain, type, protocol);
}

static trace_file_t trace_init = {
	.open = init_open,
	.close = init_close,
	.dup2 = init_dup2,
	.socket = init_socket,
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


