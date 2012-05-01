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
 * @file sp_rtrace_shmsysv.c
 *
 * Shared memory segment tracking module implementation.
 *
 * This module tracks shared memory segment creation and destruction by the
 * current process.
 */

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "sp_rtrace_main.h"
#include "sp_rtrace_module.h"
#include "common/sp_rtrace_proto.h"
#include "common/htable.h"


/* Module information */
static sp_rtrace_module_info_t module_info = {
		.type = MODULE_TYPE_PRELOAD,
		.version_major = 1,
		.version_minor = 0,
		.name = "shmsysv",
		.description = "Shared memory segment tracking module. "
				       "Tracks shared memory segment creation and destruction by "
				       "the current process.",
};

static module_resource_t res_segment = {
		.type = "segment",
		.desc = "shared memory segment",
		.flags = SP_RTRACE_RESOURCE_REFCOUNT,
};

static module_resource_t res_address = {
		.type = "address",
		.desc = "shared memory attachments",
		.flags = SP_RTRACE_RESOURCE_DEFAULT,
};

static module_resource_t res_control = {
		.type = "control",
		.desc = "shared memory segment control operation",
		.flags = SP_RTRACE_RESOURCE_DEFAULT,
};

#
#ifdef __amd64__
 #define IPC_64  0x00
#else
 #define IPC_64 	  0x100
#endif

/*
 * Mapped address to segment id mapping support.
 *
 * It is necessary to locally store addr->shmid associations
 * so we can find out the shared memory segment in shmdt
 * function.
 */

#define HASH_BITS  8
#define HASH_SIZE  (1 << HASH_BITS)
#define HASH_MASK  (HASH_SIZE - 1)


/* addr -> shmid mapping */
typedef struct {
	/* hash table support */
	htable_node_t node;
	/* the mapped address */
	const void* addr;
	/* the shared memory segment id */
	int shmid;
} addrmap_t;

static htable_t addr2shmid;


/**
 * Compare two addr->shmid mapping nodes.
 *
 * The resources are compared by comparing their mapped addresses.
 * @param[in] res1  the first node.
 * @param[in] res2  the second node.
 * @return          <0 - the first backtrace is 'less' than second.
 *                   0 - the backtraces are equal.
 *                  >0 - the first backtrace is 'greater' than second.
 */
static long addrmap_compare_nodes(const addrmap_t* node1, const addrmap_t* node2)
{
	return (long)node1->addr - (long)node2->addr;
}

/**
 * Calculates hash value for the mapping node.
 *
 * @param[in] res   the mapping node.
 * @return          the hash value.
 */
static long addrmap_calc_node_hash(const addrmap_t* node)
{
	unsigned long hash = 0;
	unsigned long value = (unsigned long)node->addr;
	while (value) {
		hash ^= value & HASH_MASK;
		value >>= 3;
	}
	return hash;
}


/* target function prototypes */
typedef int (*shmget_t)(key_t key, size_t size, int shmflg);
typedef int (*shmctl_t)(int shmid, int cmd, struct shmid_ds *buf);
typedef void* (*shmat_t)(int shmid, const void *shmaddr, int shmflg);
typedef int (*shmdt_t)(const void *shmaddr);


/**
 * Target function references.
 */
typedef struct trace_t {
	shmget_t shmget;
	shmctl_t shmctl;
	shmat_t shmat;
	shmdt_t shmdt;
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
			htable_init(&addr2shmid, HASH_SIZE, (op_unary_t)addrmap_calc_node_hash, (op_binary_t)addrmap_compare_nodes);
			trace_off.shmget = (shmget_t)dlsym(RTLD_NEXT, "shmget");
			trace_off.shmctl = (shmctl_t)dlsym(RTLD_NEXT, "shmctl");
			trace_off.shmdt = (shmdt_t)dlsym(RTLD_NEXT, "shmdt");
			trace_off.shmat = (shmat_t)dlsym(RTLD_NEXT, "shmat");
			init_mode = MODULE_LOADED;

			LOG("module loaded: %s (%d.%d)", module_info.name, module_info.version_major, module_info.version_minor);
		}

		case MODULE_LOADED: {
			if (sp_rtrace_initialize()) {
				sp_rtrace_register_module(module_info.name, module_info.version_major, module_info.version_minor, enable_tracing);
				sp_rtrace_register_resource(&res_segment);
				sp_rtrace_register_resource(&res_address);
				sp_rtrace_register_resource(&res_control);
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

static int trace_shmget(key_t key, size_t size, int shmflg)
{
	int rc = trace_off.shmget(key, size, shmflg);
	if (rc != -1 && (shmflg & IPC_CREAT)) {
		module_fcall_t call = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type_id = res_segment.id,
				.name = "shmget",
				.res_size = size,
				.res_id = (pointer_t)rc,
		};
		sp_rtrace_write_function_call(&call, NULL, NULL);
	}
	return rc;
}

static int trace_shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
	/* check if the segment was allocated by current process
	 * and read it's attachment counter if so */
	int nattach = -1;

	/* if the IPC_RMID command was issued, first read segment attachment
	 * count. We are interested only in segments created by current process */
	if (cmd == IPC_RMID) {
		struct shmid_ds ds;
		if (trace_off.shmctl(shmid, IPC_STAT | IPC_64, &ds) == 0) {
			if (ds.shm_cpid == getpid()) {
				nattach = ds.shm_nattch;
			}
		}
		module_fcall_t call = {
				.type = SP_RTRACE_FTYPE_FREE,
				.res_type_id = res_control.id,
				.name = "shmctl",
				.res_size = 0,
				.res_id = (pointer_t)shmid,
		};
		module_farg_t args[] = {
				{.name = "cmd", .value = "IPC_RMID"},
				{.name = NULL, .value = NULL}
		};
		sp_rtrace_write_function_call(&call, NULL, args);
	}
	int rc = trace_off.shmctl(shmid, cmd, buf);
	if (rc != -1 && nattach == 0) {
		/* IPC_RMID command issued to segment with attachment counter 0.
		 * This means that the segment is getting destroyed. It was created
		 * by current process, so report its deallocation. */
		module_fcall_t call = {
				.type = SP_RTRACE_FTYPE_FREE,
				.res_type_id = res_segment.id,
				.name = "shmctl",
				.res_size = 0,
				.res_id = (pointer_t)shmid,
		};
		sp_rtrace_write_function_call(&call, NULL, NULL);
	}
	return rc;
}

static void* trace_shmat(int shmid, const void *shmaddr, int shmflg)
{
	void* rc = trace_off.shmat(shmid, shmaddr, shmflg);
	int size = 1;
	if (rc != (void*)-1) {
		struct shmid_ds ds;
		/* use segment information as memory attachment parameters */
		char shmid_s[100], cpid_s[100] = {0};
		sprintf(shmid_s, "0x%x", shmid);

		if (trace_off.shmctl(shmid, IPC_STAT | IPC_64, &ds) == 0) {
			sprintf(cpid_s, "%d", ds.shm_cpid);
			size = ds.shm_segsz;

			/* check if the segment was created by current process */
			if (ds.shm_cpid == getpid()) {
				/* store addr->shmid mapping */
				addrmap_t* node = htable_create_node(sizeof(addrmap_t));
				node->shmid = shmid;
				node->addr = rc;
				node = htable_store(&addr2shmid, (void*)node);
				if (node) {
					/* TODO: Warning about overwriting already existing address ?
					 * In theory it shouldn't happen, but who knows... */
					free(node);
				}
			}
		}
		module_fcall_t call = {
				.type = SP_RTRACE_FTYPE_ALLOC,
				.res_type_id = res_address.id,
				.name = "shmat",
				.res_size = size,
				.res_id = (pointer_t)rc,
		};
		module_farg_t args[] = {
				{.name = "shmid", .value = shmid_s},
				{.name = "cpid", .value = cpid_s},
				{.name = NULL, .value = NULL}
		};
		sp_rtrace_write_function_call(&call, NULL, args);
	}
	return rc;
}


static int trace_shmdt(const void *shmaddr)
{
	int nattach = -1;
	int shmid = 0;
	addrmap_t node = {.addr = shmaddr};
	addrmap_t* pnode = (addrmap_t*)htable_find(&addr2shmid, (void*)&node);
	if (pnode != NULL) {
		shmid = pnode->shmid;
		/* if segment is marked for destruction, read its attachment counter */
		struct shmid_ds ds;
		if (trace_off.shmctl(shmid, IPC_STAT | IPC_64, &ds) == 0 && ds.shm_perm.mode & SHM_DEST) {
			nattach = ds.shm_nattch;
		}
		htable_remove_node((void*)pnode);
		free(pnode);
	}
	int rc = trace_off.shmdt(shmaddr);

	if (rc == 0) {
		/* report shared memory detachment */
		module_fcall_t call = {
				.type = SP_RTRACE_FTYPE_FREE,
				.res_type_id = res_address.id,
				.name = "shmdt",
				.res_size = 0,
				.res_id = (pointer_t)shmaddr,
		};
		sp_rtrace_write_function_call(&call, NULL, NULL);

		/* if the segment was marked for removal it should be destroyed after detaching the
		 * last address. */
		if (nattach == 1) {
			module_fcall_t call2 = {
					.type = SP_RTRACE_FTYPE_FREE,
					.res_type_id = res_segment.id,
					.name = "shmdt",
					.res_size = 0,
					.res_id = (pointer_t)shmid,
			};
			sp_rtrace_write_function_call(&call2, NULL, NULL);
		}
	}
	return rc;
}

static trace_t trace_on = {
	.shmget = trace_shmget,
	.shmctl = trace_shmctl,
	.shmdt = trace_shmdt,
	.shmat = trace_shmat,
};

/*
 * Target functions.
 */

int shmget(key_t key, size_t size, int shmflg)
{
	return trace_rt->shmget(key, size, shmflg);
}

int shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
	return trace_rt->shmctl(shmid, cmd, buf);
}

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

static int init_shmget(key_t key, size_t size, int shmflg)
{
	trace_initialize();
	return trace_init_rt->shmget(key, size, shmflg);
}

static int init_shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
	trace_initialize();
	return trace_init_rt->shmctl(shmid, cmd, buf);
}

static void* init_shmat(int shmid, const void *shmaddr, int shmflg)
{
	trace_initialize();
	return trace_init_rt->shmat(shmid, shmaddr, shmflg);
}

static int init_shmdt(const void *shmaddr)
{
	trace_initialize();
	return trace_init_rt->shmdt(shmaddr);
}

static trace_t trace_init = {
	.shmget = init_shmget,
	.shmctl = init_shmctl,
	.shmdt = init_shmdt,
	.shmat = init_shmat,
};

/*
 * Library initialization/deinitialization
 */

static void trace_shmem_init(void) __attribute__((constructor));
static void trace_shmem_fini(void) __attribute__((destructor));

static void trace_shmem_init(void)
{
	trace_initialize();
}


static void trace_shmem_fini(void)
{
	enable_tracing(false);
	htable_free(&addr2shmid, (op_unary_t)free);
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

