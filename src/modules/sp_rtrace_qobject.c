
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
 * @file sp_rtrace_qobject.c
 *
 * QObject constructor/destructor tracker.
 *
 * This module uses 'dirty hack' by directly loading and using
 * the mangled QObject constructors and destructors.
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
		MODULE_TYPE_PRELOAD,
		1,
		0,
		"qobject",
		"QObject tracking module. Tracks construction and destruction of QObject based class instances.",
};

#define QOBJECT_RES_SIZE	1

static sp_rtrace_resource_t res_qobject = {
		.type = "qobject",
		.desc = "QObject instance",
		.flags = SP_RTRACE_RESOURCE_DEFAULT,
};

/* target function prototypes */
typedef void (*qobject_dtor0_t)(void* self);
typedef void (*qobject_dtor1_t)(void* self);
typedef void (*qobject_dtor2_t)(void* self);

typedef void (*qobject_ctor1_t)(void* self, void* parent);
typedef void (*qobject_ctor1_char_t)(void* self, void* parent, const char* arg);
typedef void (*qobject_ctor1_private_t)(void* self, void* priv, void* parent);

typedef void (*qobject_ctor2_t)(void* self, void* parent);
typedef void (*qobject_ctor2_char_t)(void* self, void* parent, const char* arg);
typedef void (*qobject_ctor2_private_t)(void* self, void* priv, void* parent);

/**
 * Target function references.
 */
typedef struct trace_t {
	qobject_dtor0_t qobject_dtor0;
	qobject_dtor1_t qobject_dtor1;
	qobject_dtor2_t qobject_dtor2;
	
	qobject_ctor1_t qobject_ctor1;
	qobject_ctor1_char_t qobject_ctor1_char;
	qobject_ctor1_private_t qobject_ctor1_priv;

	qobject_ctor2_t qobject_ctor2;
	qobject_ctor2_char_t qobject_ctor2_char;
	qobject_ctor2_private_t qobject_ctor2_priv;
	
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

#define DUMP_TRACEOFF_FIELD(field) fprintf(stderr, "\ttrace_off." #field "=%p\n", trace_off.field);

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
			
			trace_off.qobject_dtor0 = (qobject_dtor0_t)dlsym(RTLD_NEXT, "_ZN7QObjectD0Ev");
			trace_off.qobject_dtor1 = (qobject_dtor1_t)dlsym(RTLD_NEXT, "_ZN7QObjectD1Ev");
			trace_off.qobject_dtor2 = (qobject_dtor2_t)dlsym(RTLD_NEXT, "_ZN7QObjectD2Ev");
			
			trace_off.qobject_ctor1 = (qobject_ctor1_t)dlsym(RTLD_NEXT, "_ZN7QObjectC1EPS_");
			trace_off.qobject_ctor1_char = (qobject_ctor1_char_t)dlsym(RTLD_NEXT, "_ZN7QObjectC1EPS_PKc");
			trace_off.qobject_ctor1_priv = (qobject_ctor1_private_t)dlsym(RTLD_NEXT, "_ZN7QObjectC1ER14QObjectPrivatePS_");

			trace_off.qobject_ctor2 = (qobject_ctor2_t)dlsym(RTLD_NEXT, "_ZN7QObjectC2EPS_");
			trace_off.qobject_ctor2_char = (qobject_ctor2_char_t)dlsym(RTLD_NEXT, "_ZN7QObjectC2EPS_PKc");
			trace_off.qobject_ctor2_priv = (qobject_ctor2_private_t)dlsym(RTLD_NEXT, "_ZN7QObjectC2ER14QObjectPrivatePS_");
			
			init_mode = MODULE_LOADED;

			LOG("module loaded: %s (%d.%d)", module_info.name, module_info.version_major, module_info.version_minor);
		}

		case MODULE_LOADED: {
			if (sp_rtrace_initialize()) {
				sp_rtrace_register_module(module_info.name, module_info.version_major, module_info.version_minor, enable_tracing);
				sp_rtrace_register_resource(&res_qobject);
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

static void trace_qobject_dtor0(void* self)
{
	trace_off.qobject_dtor0(self);

	sp_rtrace_fcall_t call = {
			.type = SP_RTRACE_FTYPE_FREE,
			.res_type = (void*)res_qobject.id,
			.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
			.name = "QObject::~QObject",
			.res_size = 0,
			.res_id = (pointer_t)self,
	};
	sp_rtrace_write_function_call(&call, NULL);
}

static void trace_qobject_dtor1(void* self)
{
	trace_off.qobject_dtor1(self);

	sp_rtrace_fcall_t call = {
			.type = SP_RTRACE_FTYPE_FREE,
			.res_type = (void*)res_qobject.id,
			.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
			.name = "QObject::~QObject",
			.res_size = 0,
			.res_id = (pointer_t)self,
	};
	sp_rtrace_write_function_call(&call, NULL);
}

static void trace_qobject_dtor2(void* self)
{
	trace_off.qobject_dtor2(self);

	sp_rtrace_fcall_t call = {
			.type = SP_RTRACE_FTYPE_FREE,
			.res_type = (void*)res_qobject.id,
			.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
			.name = "QObject::~QObject",
			.res_size = 0,
			.res_id = (pointer_t)self,
	};
	sp_rtrace_write_function_call(&call, NULL);
}


static void trace_qobject_ctor1(void* self, void* parent)
{
	trace_off.qobject_ctor1(self, parent);

	sp_rtrace_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type = (void*)res_qobject.id,
			.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
			.name = "QObject::QObject",
			.res_size = QOBJECT_RES_SIZE,
			.res_id = (pointer_t)self,
	};
	sp_rtrace_write_function_call(&call, NULL);
}

static void trace_qobject_ctor1_char(void* self, void* parent, const char* arg)
{
	trace_off.qobject_ctor1_char(self, parent, arg);

	sp_rtrace_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type = (void*)res_qobject.id,
			.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
			.name = "QObject::QObject",
			.res_size = QOBJECT_RES_SIZE,
			.res_id = (pointer_t)self,
	};
	sp_rtrace_write_function_call(&call, NULL);
}

static void trace_qobject_ctor1_priv(void* self, void* priv, void* parent)
{
	trace_off.qobject_ctor1_priv(self, priv, parent);

	sp_rtrace_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type = (void*)res_qobject.id,
			.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
			.name = "QObject::QObject",
			.res_size = QOBJECT_RES_SIZE,
			.res_id = (pointer_t)self,
	};
	sp_rtrace_write_function_call(&call, NULL);
}


static void trace_qobject_ctor2(void* self, void* parent)
{
	trace_off.qobject_ctor2(self, parent);

	sp_rtrace_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type = (void*)res_qobject.id,
			.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
			.name = "QObject::QObject",
			.res_size = QOBJECT_RES_SIZE,
			.res_id = (pointer_t)self,
	};
	sp_rtrace_write_function_call(&call, NULL);
}

static void trace_qobject_ctor2_char(void* self, void* parent, const char* arg)
{
	trace_off.qobject_ctor2_char(self, parent, arg);

	sp_rtrace_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type = (void*)res_qobject.id,
			.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
			.name = "QObject::QObject",
			.res_size = QOBJECT_RES_SIZE,
			.res_id = (pointer_t)self,
	};
	sp_rtrace_write_function_call(&call, NULL);
}

static void trace_qobject_ctor2_priv(void* self, void* priv, void* parent)
{
	trace_off.qobject_ctor2_priv(self, priv, parent);

	sp_rtrace_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type = (void*)res_qobject.id,
			.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
			.name = "QObject::QObject",
			.res_size = QOBJECT_RES_SIZE,
			.res_id = (pointer_t)self,
	};
	sp_rtrace_write_function_call(&call, NULL);
}

static trace_t trace_on = {
	trace_qobject_dtor0,
	trace_qobject_dtor1,
	trace_qobject_dtor2,
	
	trace_qobject_ctor1,
	trace_qobject_ctor1_char,
	trace_qobject_ctor1_priv,

	trace_qobject_ctor2,
	trace_qobject_ctor2_char,
	trace_qobject_ctor2_priv,
};

/*
 * Target functions.
 */


void _ZN7QObjectD0Ev(void* self)
{
	trace_rt->qobject_dtor0(self);
}

void _ZN7QObjectD1Ev(void* self)
{
	trace_rt->qobject_dtor1(self);
}

void _ZN7QObjectD2Ev(void* self)
{
	trace_rt->qobject_dtor2(self);
}


void _ZN7QObjectC1EPS_(void* self, void* parent)
{
	trace_rt->qobject_ctor1(self, parent);
}

void _ZN7QObjectC1EPS_PKc(void* self, void* parent, const char* arg)
{
	trace_rt->qobject_ctor1_char(self, parent, arg);
}

void _ZN7QObjectC1ER14QObjectPrivatePS_(void* self, void* priv, void* parent)
{
	trace_rt->qobject_ctor1_priv(self, priv, parent);
}


void _ZN7QObjectC2EPS_(void* self, void* parent)
{
	trace_rt->qobject_ctor2(self, parent);
}

void _ZN7QObjectC2EPS_PKc(void* self, void* parent, const char* arg)
{
	trace_rt->qobject_ctor2_char(self, parent, arg);
}

void _ZN7QObjectC2ER14QObjectPrivatePS_(void* self, void* priv, void* parent)
{
	trace_rt->qobject_ctor2_priv(self, priv, parent);
}


/*
 * Initialization functions.
 */

static void init_qobject_dtor0(void* self)
{
	trace_initialize();
	trace_init_rt->qobject_dtor0(self);
}

static void init_qobject_dtor1(void* self)
{
	trace_initialize();
	trace_init_rt->qobject_dtor1(self);
}

static void init_qobject_dtor2(void* self)
{
	trace_initialize();
	trace_init_rt->qobject_dtor2(self);
}


static void init_qobject_ctor1(void* self, void* parent)
{
	trace_initialize();
	trace_init_rt->qobject_ctor1(self, parent);
}

static void init_qobject_ctor1_char(void* self, void* parent, const char* arg)
{
	trace_initialize();
	trace_init_rt->qobject_ctor1_char(self, parent, arg);
}

static void init_qobject_ctor1_priv(void* self, void* priv, void* parent)
{
	trace_initialize();
	trace_init_rt->qobject_ctor1_priv(self, priv, parent);
}


static void init_qobject_ctor2(void* self, void* parent)
{
	trace_initialize();
	trace_init_rt->qobject_ctor2(self, parent);
}

static void init_qobject_ctor2_char(void* self, void* parent, const char* arg)
{
	trace_initialize();
	trace_init_rt->qobject_ctor2_char(self, parent, arg);
}

static void init_qobject_ctor2_priv(void* self, void* priv, void* parent)
{
	trace_initialize();
	trace_init_rt->qobject_ctor2_priv(self, priv, parent);
}

static trace_t trace_init = {
	init_qobject_dtor0,
	init_qobject_dtor1,
	init_qobject_dtor2,
	
	init_qobject_ctor1,
	init_qobject_ctor1_char,
	init_qobject_ctor1_priv,

	init_qobject_ctor2,
	init_qobject_ctor2_char,
	init_qobject_ctor2_priv,
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

