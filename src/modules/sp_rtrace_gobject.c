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
 * @file sp_rtrace_gobject.c
 *
 * This module tracks GObject references.
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <unistd.h>
#include <glib-object.h>

#include "sp_rtrace_main.h"
#include "sp_rtrace_module.h"

#include "common/sp_rtrace_proto.h"

/* Module information */
static sp_rtrace_module_info_t module_info = {
		.type = MODULE_TYPE_PRELOAD,
		.version_major = 1,
		.version_minor = 0,
		.name = "gobject",
		.description = "GObject tracking module. Tracks GObject references.",
};


static sp_rtrace_resource_t res_gobject = {
		.type = "gobject",
		.desc = "GObject instance",
		.flags = SP_RTRACE_RESOURCE_REFCOUNT,
};


 /*
  * file module function set
  */
  
typedef gpointer (*g_object_newv_t)(GType object_type, guint n_parameters, GParameter* parameters);
typedef void (*g_type_free_instance_t)(GTypeInstance* instance);
typedef gpointer (*g_object_ref_t)(gpointer object);
typedef void (*g_object_unref_t)(gpointer object);

typedef struct {
	g_object_newv_t g_object_newv;
	g_type_free_instance_t g_type_free_instance;
	g_object_ref_t g_object_ref;
	g_object_unref_t g_object_unref;
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
			trace_off.g_object_newv = (g_object_newv_t)dlsym(RTLD_NEXT, "g_object_newv");
			trace_off.g_type_free_instance = (g_type_free_instance_t)dlsym(RTLD_NEXT, "g_type_free_instance");
			trace_off.g_object_ref = (g_object_ref_t)dlsym(RTLD_NEXT, "g_object_ref");
			trace_off.g_object_unref = (g_object_unref_t)dlsym(RTLD_NEXT, "g_object_unref");
			init_mode = MODULE_LOADED;

			LOG("module loaded: %s (%d.%d)", module_info.name, module_info.version_major, module_info.version_minor);
		}

		case MODULE_LOADED: {
			if (sp_rtrace_initialize()) {
				sp_rtrace_register_module(module_info.name, module_info.version_major, module_info.version_minor, enable_tracing);
				sp_rtrace_register_resource(&res_gobject);
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
static gpointer trace_g_object_newv(GType object_type, guint n_parameters, GParameter* parameters)
{
	gpointer rc = trace_off.g_object_newv(object_type, n_parameters, parameters);
	pointer_t res_id = (pointer_t)rc;
	size_t res_size = (size_t)1;
	const char *type_name = "unknown";
	GTypeQuery type_info;

	g_type_query(object_type, &type_info);
	if (type_info.type) {
		res_size = type_info.instance_size;
		type_name = type_info.type_name;
	}

	sp_rtrace_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type = (void*)res_gobject.id,
			.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
			.name = "g_object_newv",
			.res_size = res_size,
			.res_id = (pointer_t)res_id,
	};
	sp_rtrace_farg_t args[] = {
			{.name = "type", .value = type_name},
			{.name = NULL, .value = NULL}
	};
	sp_rtrace_write_function_call(&call, NULL, args);
	return rc;
}


static void trace_g_type_free_instance(GTypeInstance* instance)
{
	trace_off.g_type_free_instance(instance);

	sp_rtrace_fcall_t call = {
			.type = SP_RTRACE_FTYPE_FREE,
			.res_type = (void*)res_gobject.id,
			.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
			.name = "g_type_free_instance",
			.res_size = 0,
			.res_id = (pointer_t)(pointer_t)instance,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
}


static gpointer trace_g_object_ref(gpointer object)
{
	gpointer rc = trace_off.g_object_ref(object);
	pointer_t res_id = (pointer_t)rc;
	size_t res_size = (size_t)1;
	const char *type_name = "unknown";
	GTypeQuery type_info;

	g_type_query(((GObject*)object)->g_type_instance.g_class->g_type, &type_info);
	if (type_info.type) {
		res_size = type_info.instance_size;
		type_name = type_info.type_name;
	}
	sp_rtrace_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type = (void*)res_gobject.id,
			.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
			.name = "g_object_ref",
			.res_size = res_size,
			.res_id = (pointer_t)res_id,
	};
	sp_rtrace_farg_t args[] = {
			{.name = "type", .value = type_name},
			{.name = NULL, .value = NULL}
	};
	sp_rtrace_write_function_call(&call, NULL, args);

	return rc;
}


static void trace_g_object_unref(gpointer object)
{
	trace_off.g_object_unref(object);
	
	sp_rtrace_fcall_t call = {
			.type = SP_RTRACE_FTYPE_FREE,
			.res_type = (void*)res_gobject.id,
			.res_type_flag = SP_RTRACE_FCALL_RFIELD_ID,
			.name = "g_object_unref",
			.res_size = 0,
			.res_id = (pointer_t)(pointer_t)object,
	};
	sp_rtrace_write_function_call(&call, NULL, NULL);
}



static trace_t trace_on = {
	.g_object_newv = trace_g_object_newv,
	.g_type_free_instance = trace_g_type_free_instance,
	.g_object_ref = trace_g_object_ref,
	.g_object_unref = trace_g_object_unref,
};


/* target functions */
gpointer g_object_newv(GType object_type, guint n_parameters, GParameter* parameters)
{
	gpointer rc;
	rc = trace_rt->g_object_newv(object_type, n_parameters, parameters);
	return rc;
}


void g_type_free_instance(GTypeInstance* instance)
{
	
	trace_rt->g_type_free_instance(instance);
	
}


gpointer g_object_ref(gpointer object)
{
	gpointer rc;
	rc = trace_rt->g_object_ref(object);
	return rc;
}


void g_object_unref(gpointer object)
{
	
	trace_rt->g_object_unref(object);
	
}


/*
 * Initialization functions.
 */
static gpointer init_g_object_newv(GType object_type, guint n_parameters, GParameter* parameters)
{
	trace_initialize();
	gpointer rc = trace_init_rt->g_object_newv(object_type, n_parameters, parameters);
	return rc;
}
static void init_g_type_free_instance(GTypeInstance* instance)
{
	trace_initialize();
	trace_init_rt->g_type_free_instance(instance);
	
}
static gpointer init_g_object_ref(gpointer object)
{
	trace_initialize();
	gpointer rc = trace_init_rt->g_object_ref(object);
	return rc;
}
static void init_g_object_unref(gpointer object)
{
	trace_initialize();
	trace_init_rt->g_object_unref(object);
	
}

static trace_t trace_init = {
	.g_object_newv = init_g_object_newv,
	.g_type_free_instance = init_g_type_free_instance,
	.g_object_ref = init_g_object_ref,
	.g_object_unref = init_g_object_unref,
};

/* */

/*
 * Library initialization/deinitialization
 */

static void trace_init_lib(void) __attribute__((constructor));
static void trace_fini_lib(void) __attribute__((destructor));

static void trace_init_lib(void)
{
	trace_initialize();
}


static void trace_fini_lib(void)
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
