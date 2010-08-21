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
 * @file sp_rtrace_memshared.c
 *
 * Shared memory attaching/detaching tracking module implementation.
 *
 * This module tracks shared memory segment attach/detach calls
 * (shmat, shmdt functions).
 */

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>


#include "sp_rtrace_main.h"
#include "common/sp_rtrace_proto.h"
#include "common/htable.h"


/* Module information */
static sp_rtrace_module_info_t module_info = {
		.type = MODULE_TYPE_PRELOAD,
		.version_major = 1,
		.version_minor = 0,
		.name = "shmaddr",
		.description = "Shared memory attachment tracking module. "
				       "Tracks shared memory segment attaching/detaching with shmat/shmdt functions.",
};

/* the module identifier assigned by main module */
static int module_id = 0;

/* target function prototypes */
typedef void* (*shmat_t)(int shmid, const void *shmaddr, int shmflg);
typedef int (*shmdt_t)(const void *shmaddr);


/**
 * Target function references.
 */
typedef struct trace_t {
	shmat_t shmat;
	shmdt_t shmdt;
} trace_t;


/* original function references */
static trace_t trace_off = {0};
/* tracing function references */
static trace_t trace_on;

static trace_t trace_init;

/* Runtime function references */
static trace_t* trace_rt = &trace_init;

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
	trace_off.shmdt = (shmdt_t)dlsym(RTLD_NEXT, "shmdt");
	trace_off.shmat = (shmat_t)dlsym(RTLD_NEXT, "shmat");

	enable_tracing(false);
}

/*
 * tracing functions
 */
void* trace_shmat(int shmid, const void *shmaddr, int shmflg)
{
	void* rc = trace_off.shmat(shmid, shmaddr, shmflg);
	if (rc) {
		/* prepare arguments data to hold shmid and the creator
		 * process id (it's not exactly argument, but useful
		 * information nevertheless. */
		char arg1[100], arg2[100];
		char* args[] = {arg1, NULL, NULL};
		sprintf(arg1, "shmid=0x%x", shmid);
		struct shmid_ds ds;
		if (shmctl(shmid, IPC_STAT, &ds) == 0) {
			sprintf(arg2, "shmid_ds::shm_cpid=%d", ds.shm_cpid);
			args[1] = arg2;
		}
		sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "shmat", 1, rc, args);
	}
	return rc;
}

int trace_shmdt(const void *shmaddr)
{
	int rc = trace_off.shmdt(shmaddr);
	if (rc == 0) {
		sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_FREE, "shdt", 0, shmaddr, NULL);
	}
	return rc;
}

static trace_t trace_on = {
	.shmdt = trace_shmdt,
	.shmat = trace_shmat,
};

/*
 * Target functions.
 */


void* shmat(int shmid, const void *shmaddr, int shmflg)
{
	return trace_rt->shmat(shmid, shmaddr, shmflg);
}

int shmdt(const void *shmaddr)
{
	return trace_rt->shmdt(shmaddr);
}


/*
 * Initialization functions.
 */


static void* init_shmat(int shmid, const void *shmaddr, int shmflg)
{
	trace_initialize();
	return trace_off.shmat(shmid, shmaddr, shmflg);
}

static int init_shmdt(const void *shmaddr)
{
	trace_initialize();
	return trace_off.shmdt(shmaddr);
}

static trace_t trace_init = {
	.shmdt = init_shmdt,
	.shmat = init_shmat,
};

/*
 * Library initialization/deinitialization
 */

static void trace_shmaddr_init(void) __attribute__((constructor));
static void trace_shmaddr_fini(void) __attribute__((destructor));

static void trace_shmaddr_init(void)
{
	LOG("initializing %s (%d.%d)", module_info.name, module_info.version_major, module_info.version_minor);
	trace_initialize();
	module_id = sp_rtrace_register_module(module_info.name, module_info.version_major, module_info.version_minor, enable_tracing);
}

static void trace_shmaddr_fini(void)
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

