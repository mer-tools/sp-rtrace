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
 * Shared memory tracking module implementation.
 *
 * This module tracks allocations of the shared memory segments with
 * shmget and their destruction.
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
		.name = "memshared",
		.description = "Shared memory segment creation/destruction tracing module. "
				       "Tracks shared memory segment creation with shmget function and "
				       "their destruction.",
};

/* the module identifier assigned by main module */
static int module_id = 0;

/*
 * Mapped address to segment id mapping support.
 *
 * It is necessary to locally store addr->shmid associations
 * so we can find out the shared memory segment in shmdt
 * function.
 */

#define HASH_BITS  15
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
} map_t;
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
static long compare_nodes(const map_t* node1, const map_t* node2)
{
	return node1->addr - node2->addr;
}

/**
 * Calculates hash value for the mapping node.
 *
 * @param[in] res   the mapping node.
 * @return          the hash value.
 */
static long calc_node_hash(const map_t* node)
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
	trace_off.shmget = (shmget_t)dlsym(RTLD_NEXT, "shmget");
	trace_off.shmctl = (shmctl_t)dlsym(RTLD_NEXT, "shmctl");
	trace_off.shmdt = (shmdt_t)dlsym(RTLD_NEXT, "shmdt");
	trace_off.shmat = (shmat_t)dlsym(RTLD_NEXT, "shmat");

	enable_tracing(false);
}

/*
 * tracing functions
 */

int trace_shmget(key_t key, size_t size, int shmflg)
{
	int rc = trace_off.shmget(key, size, shmflg);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "shmget", size, rc, NULL);
	return rc;
}

int trace_shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
	int rc = trace_off.shmctl(shmid, cmd, buf);
	sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "shmctl", 1, rc, NULL);
	return rc;
}

void* trace_shmat(int shmid, const void *shmaddr, int shmflg)
{
	void* rc = trace_off.shmat(shmid, shmaddr, shmflg);
	if (rc) {
		/* store addr->shmid mapping */
		map_t* node = htable_create_node(sizeof(map_t));
		node->shmid = shmid;
		node->addr = rc;
		node = htable_store(&addr2shmid, (void*)node);
		if (node) {
			/* TODO: Warning about overwriting already existing address ?
			 * In theory it shouldn't happen, but who knows... */
			free(node);
		}
		sp_rtrace_write_function_call(module_id, SP_RTRACE_FTYPE_ALLOC, "shmat", 1, rc, NULL);
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
	return trace_off.shmget(key, size, shmflg);
}

static int init_shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
	trace_initialize();
	return trace_off.shmctl(shmid, cmd, buf);
}

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
	LOG("initializing %s (%d.%d)", module_info.name, module_info.version_major, module_info.version_minor);
	htable_init(&addr2shmid, HASH_SIZE, (op_unary_t)calc_node_hash, (op_binary_t)compare_nodes);
	trace_initialize();

	module_id = sp_rtrace_register_module(module_info.name, module_info.version_major, module_info.version_minor, enable_tracing);
}


static long dump_mapping(map_t* node)
{
	fprintf(stderr, "%p -> 0x%x\n", node->addr, node->shmid);
}

static void trace_shmem_fini(void)
{
	enable_tracing(false);
//	htable_foreach(&addr2shmid, (op_unary_t)dump_mapping);
	htable_free(&addr2shmid, (op_unary_t)free);
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

