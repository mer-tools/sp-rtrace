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
 * @file sp_rtrace_shmposix.c
 *
 * Posix shared memory tracking module.
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <search.h>

#include "sp_rtrace_main.h"
#include "sp_rtrace_module.h"

#include "common/sp_rtrace_proto.h"

#define ALIGN_TO_4KB (size) (((size)+4095) & 0xFFFFFC00)

/* some of the info is needed also afterwards,
 * so this is disabled for now
 */
#define DO_CLEANUP 0

/* mmap2() is a syscall, not Glibc call, disable it */
#define TRACE_MMAP2 0

/*
 * file module function set
 */
  
typedef int (*shm_open_t)(const char *name, int oflag, mode_t mode);
typedef int (*shm_unlink_t)(const char *name);
typedef int (*open_t)(const char *pathname, int flags, ...);
typedef int (*open64_t)(const char *pathname, int flags, ...);
typedef int (*creat_t)(const char *pathname, mode_t mode);
typedef void* (*mmap_t)(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
typedef void* (*mmap2_t)(void *addr, size_t length, int prot, int flags, int fd, off_t pgoffset);
typedef void* (*mmap64_t)(void *addr, size_t length, int prot, int flags, int fd, off64_t offset);
typedef int (*munmap_t)(void *addr, size_t length);
typedef int (*close_t)(int fd);

typedef struct {
	shm_open_t shm_open;
	shm_unlink_t shm_unlink;
	open_t open;
	open64_t open64;
	creat_t creat;
	mmap_t mmap;
#if TRACE_MMAP2
	mmap2_t mmap2;
#endif
	mmap64_t mmap64;
	munmap_t munmap;
	close_t close;
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
	.name = "shmposix",
	.description = "Posix shared memory tracking module. Tracks mmap() etc.",
};

/* resource identifiers */
static module_resource_t res_pshmmap = {
	.type = "pshmmap",
	.desc = "posix shared memory mapping",
	.flags = SP_RTRACE_RESOURCE_DEFAULT,
};

static module_resource_t res_fshmmap = {
	.type = "fshmmap",
	.desc = "file mapping",
	.flags = SP_RTRACE_RESOURCE_DEFAULT,
};

static module_resource_t res_shmmap = {
	.type = "shmmap",
	.desc = "generic memory mapping",
	.flags = SP_RTRACE_RESOURCE_DEFAULT,
};

static module_resource_t res_pshmobj = {
	.type = "pshmobj",
	.desc = "posix shared memory object",
	.flags = SP_RTRACE_RESOURCE_DEFAULT,
};

static module_resource_t res_pshmfd = {
	.type = "pshmfd",
	.desc = "opened posix shared memory object",
	.flags = SP_RTRACE_RESOURCE_DEFAULT,
};


/*
 * Name registry implementation
 *
 * Name registry manages posix shared object name association with
 * unique (in context) resource identifier. The identifier is calculated
 * from the name hash and incremented if it conflicts with already
 * registered resource identifiers.
 */

/**
 * Name registry node
 */
typedef struct nreg_node_t {
	/* the object name */
	char* name;
	/* the unique resource identifier */
	unsigned int hash;
} nreg_node_t;

/**
 * Name registry node const version
 */
typedef struct nreg_node_const_t {
	/* the object name */
	const char* name;
	/* the unique resource identifier */
	unsigned int hash;
} nreg_node_const_t;

/* the name registry root*/
static void* nreg_root = NULL;

/* the following variables are used to store hash validation data */

/* the hash value to validate */
static unsigned int nreg_validate_hash_value = 0;
/* the hash validation result */
static bool nreg_validate_hash_result = false;
/**/

/**
 * Compares two registry nodes.
 *
 * @param[in] item1  the first node.
 * @param[in] item2  the second node.
 * @return           -1 - item1 < item2
 *                    0 - teim1 == item2
 *                    1 - item1 > item2
 */
static int nreg_compare_name(const void* item1, const void* item2)
{
	const nreg_node_t* node1 = item1;
	const nreg_node_t* node2 = item2;
	return strcmp(node1->name, node2->name);
}

/**
 * Calculates hash from the specified string.
 *
 * @param[in] name   the name to calculate cash for.
 * @return           the calculated hash code.
 */
static unsigned int nreg_calc_raw_hash(const char* name)
{
	unsigned int hash = 0, i;
	size_t len = strlen(name);
	for (i = 0; i < len; i++) {
		hash += *name++;
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

/**
 * Validates hash code.
 *
 * This function is used as action for twalk() function. It checks if the
 * nreg_validate_hash_value conflicts with an existing hash value and sets
 * nreg_validate_hash_result to false in such case.
 * @param[in] item    the item to validate.
 * @param[in] which
 * @param[in] depth
 */
static void nreg_validate_hash(const void* item, const VISIT which, __attribute__((unused)) int depth)
{
	if (nreg_validate_hash_result) {
		switch (which) {
			case preorder:
				break;

			case postorder:
			case leaf: {
				const nreg_node_t* pnode = *(nreg_node_t* const*)item;
				if (pnode->hash == nreg_validate_hash_value) nreg_validate_hash_result = false;
				break;
			}
			case endorder:
				break;
		}
	}
}

/**
 * Checks if the specified hash isn't already registered.
 * @param[in] hash  the hash value to check.
 * @return          true if the hash value is already registered, false otherwise.
 */
static bool nreg_hash_exists(unsigned int hash)
{
	nreg_validate_hash_result = true;
	nreg_validate_hash_value = hash;
	twalk(nreg_root, nreg_validate_hash);
	return !nreg_validate_hash_result;
}

/**
 * Calculates hash value (resource identifier) for the specified name.
 *
 * @param[in] name   the name.
 * @return           the name hash code (resource identifier).
 */
static unsigned int nreg_calc_hash(const char* name)
{
	/* first calculate hash for the name string */
	unsigned int hash = nreg_calc_raw_hash(name);
	/* then ensure that it doesn't conflict with other objects */
	while (nreg_hash_exists(hash)) hash++;
	return hash;
}

/**
 * Retrieves hash for the specified name.
 *
 * If the specified name is already registered, its hash code is
 * returned. Otherwise a new hash code is calculated, registered and
 * returned.
 * @param[in] name  the object name.
 * @return          the hash code.
 */
static unsigned int nreg_get_hash(const char* name)
{
	nreg_node_const_t node = {.name = name};
	nreg_node_t** ppnode = tfind(&node, &nreg_root, nreg_compare_name);

	if (!ppnode) {
		nreg_node_t* pnode = malloc(sizeof(nreg_node_t));
		if (!pnode) return -1;
		pnode->name = strdup(name);
		if (!pnode->name) {
			free(pnode);
			return -1;
		}
		pnode->hash = nreg_calc_hash(name);
		ppnode = tsearch(pnode, &nreg_root, nreg_compare_name);
	}
	return (*ppnode)->hash;
}

#if DO_CLEANUP
/**
 * Releases resource allocated for name registry node.
 *
 * @param[in] item   the name registry node to free.
 */
static void nreg_free_node(void* item)
{
	nreg_node_t* pnode = (nreg_node_t*)item;
	if (pnode->name) free(pnode->name);
	free(pnode);
}

/**
 * Releases resources allocated by name registry.
 */
static void hash_cleanup(void)
{
	tdestroy(nreg_root, nreg_free_node);
}
#endif /* DO_CLEANUP */

/*
 * File descriptor registry implementation.
 *
 * File descriptor registry is used to track the opened file descriptors
 * so more information can be provided for mmap functions.
 */

/**
 * Descriptor type enumeration
 */
enum {
	FD_UNKNOWN = 0,
	/* posix shared memory object */
	FD_POSIX = 1,
	/* file */
	FD_FILE = 2,
};

/**
 * File descriptor registry node
 */
typedef struct fdreg_node_t {
	/* file descriptor type */
	unsigned int type;
	/* file/object name */
	char* name;
	/* the decriptor value */
	int fd;
	/* the file/object open mode */
	int mode;
} fdreg_node_t;

/* the file descriptor registry root node */
static void* fdreg_root;


/**
 * Compares two file descriptor registry nodes by their file descriptors.
 *
 * @param[in] item1  the first node.
 * @param[in] item2  the second node.
 * @return           -1 - item1 < item2
 *                    0 - teim1 == item2
 *                    1 - item1 > item2
 */
static int fdreg_compare_fd(const void* item1, const void* item2)
{
	const fdreg_node_t* node1 = item1;
	const fdreg_node_t* node2 = item2;
	return node1->fd - node2->fd;
}

/**
 * Stores file descriptor into registry.
 *
 * Existing file descriptor data in registry is overwritten.
 * @param[in] fd     the file descriptor value.
 * @param[in] name   the name associated with the descriptor.
 * @param[in] type   the file descriptor type (FD_POSIX | FD_FILE).
 * @param[in] mode   the file descriptor open mode.
 */
static void fdreg_store_fd(int fd, const char* name, unsigned int type, int mode)
{
	fdreg_node_t node = {.fd = fd};
	fdreg_node_t** ppnode = tfind(&node, &fdreg_root, fdreg_compare_fd);
	if (ppnode) {
		/* file descriptor was already registered, update it's data */
		fdreg_node_t* pnode = *ppnode;
		if (pnode->name) free(pnode->name);
		pnode->name = strdup(name);
		pnode->type = type;
		pnode->mode = mode;
		return;
	}
	/* store the file descriptor into registry */
	fdreg_node_t* pnode = malloc(sizeof(fdreg_node_t));
	if (!pnode) return;
	pnode->name = strdup(name);
	pnode->type = type;
	pnode->fd = fd;
	pnode->mode = mode;
	tsearch(pnode, &fdreg_root, fdreg_compare_fd);
}

/**
 * Retrieves file descriptor data from the registry.
 * @param[in] fd  the file descriptor to retrieve data for.
 * @return        the file descriptor data or NULL.
 */
static fdreg_node_t* fdreg_get_fd(int fd)
{
	fdreg_node_t node = {.fd = fd};
	fdreg_node_t** ppnode = tfind(&node, &fdreg_root, fdreg_compare_fd);
	if (ppnode) return *ppnode;
	return NULL;
}


#if DO_CLEANUP
/**
 * Releases resources allocated for file descriptor registry node.
 *
 * @param[in] item   the file descriptor registry node.
 */
static void fdreg_free_node(void* item)
{
	fdreg_node_t* pnode = item;
	if (pnode->name) free(pnode->name);
	free(pnode);
}

/**
 * Removes file descriptor data from the registry.
 *
 * @param[in] fd  the file descriptor.
 */
static void fdreg_remove(int fd)
{
	fdreg_node_t node = {.fd = fd};
	fdreg_node_t** ppnode = tfind(&node, &fdreg_root, fdreg_compare_fd);
	if (ppnode) {
		fdreg_free_node(*ppnode);
		tdelete(&node, &fdreg_root, fdreg_compare_fd);
	}
}

/**
 * Releases resources allocated by file descriptor registry.
 */
static void fdreg_cleanup(void)
{
	tdestroy(fdreg_root, fdreg_free_node);
}
#endif /* DO_CLEANUP */


/*
 * Address mapping registry
 *
 * The address mapping registry is used to associate descriptors mapped
 * by mmap() function with returned addresses.
 */

/*
 * The address mapping node.
 */
typedef struct addr_node_t {
	/* the mapped address */
	pointer_t addr;
	/* the associated file descriptor */
	int fd;
} addr_node_t;

/* the address mapping root */
static void* addr_root;

/**
 * Compares two address mapping registry nodes.
 *
 * @param item1
 * @param item2
 * @return
 */
static int addr_compare(const void* item1, const void* item2)
{
	const addr_node_t* node1 = item1;
	const addr_node_t* node2 = item2;
	if (node1->addr < node2->addr) return -1;
	if (node1->addr > node2->addr) return 1;
	return 0;
}

/**
 * Stores new address mapping into registry.
 *
 * @param[in] addr   the new address.
 * @param[in] fd     the mapped descriptor.
 */
static void addr_store(pointer_t addr, int fd)
{
	addr_node_t node = {.addr = addr};
	addr_node_t** ppnode = tfind(&node, &addr_root, addr_compare);
	if (ppnode) {
		addr_node_t* pnode = *ppnode;
		pnode->fd = fd;
		return;
	}
	addr_node_t* pnode = malloc(sizeof(addr_node_t));
	if (!pnode) return;
	pnode->addr = addr;
	pnode->fd = fd;
	tsearch(pnode, &addr_root, addr_compare);
}

/**
 * Retrieves address mapping data from registry.
 *
 * @param[in] addr    the address to retrieve data for.
 * @return            the mapping data or NULL.
 */
static addr_node_t* addr_get(pointer_t addr)
{
	addr_node_t node = {.addr = addr};
	addr_node_t** ppnode = tfind(&node, &addr_root, addr_compare);
	if (ppnode) return *ppnode;
	return NULL;
}

#if DO_CLEANUP
/**
 * Releases resources allocated by adress mapping registry.
 *
 * @return
 */
static void addr_cleanup(void)
{
	tdestroy(addr_root, free);
}
#endif /* DO_CLEANUP */


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
			trace_off.shm_open = (shm_open_t)dlsym(RTLD_NEXT, "shm_open");
			trace_off.shm_unlink = (shm_unlink_t)dlsym(RTLD_NEXT, "shm_unlink");
			trace_off.open = (open_t)dlsym(RTLD_NEXT, "open");
			trace_off.open64 = (open64_t)dlsym(RTLD_NEXT, "open64");
			trace_off.creat = (creat_t)dlsym(RTLD_NEXT, "creat");
			trace_off.mmap = (mmap_t)dlsym(RTLD_NEXT, "mmap");
#if TRACE_MMAP2
			trace_off.mmap2 = (mmap2_t)dlsym(RTLD_NEXT, "mmap2");
#endif
			trace_off.mmap64 = (mmap64_t)dlsym(RTLD_NEXT, "mmap64");
			trace_off.munmap = (munmap_t)dlsym(RTLD_NEXT, "munmap");
			trace_off.close = (close_t)dlsym(RTLD_NEXT, "close");
			init_mode = MODULE_LOADED;

			LOG("module loaded: %s (%d.%d)", module_info.name, module_info.version_major, module_info.version_minor);
		}

		case MODULE_LOADED: {
			if (sp_rtrace_initialize()) {
				init_mode = MODULE_READY;

				sp_rtrace_register_module(&module_info, enable_tracing);
				sp_rtrace_register_resource(&res_pshmmap);
				sp_rtrace_register_resource(&res_fshmmap);
				sp_rtrace_register_resource(&res_shmmap);
				sp_rtrace_register_resource(&res_pshmobj);
				sp_rtrace_register_resource(&res_pshmfd);
				trace_init_rt = trace_rt;

				LOG("module ready: %s (%d.%d)", module_info.name, module_info.version_major, module_info.version_minor);
			}
		}
	}
}

/*
 * tracing functions
 */
static int trace_shm_open(const char *name, int oflag, mode_t mode)
{
	int rc = trace_off.shm_open(name, oflag, mode);
	if (rc < 0) {
		return rc;
	}
	fdreg_store_fd(rc, name, FD_POSIX, oflag);

	char arg_oflag[16]; snprintf(arg_oflag, sizeof(arg_oflag), "0x%x", oflag);
	char arg_mode[16]; snprintf(arg_mode, sizeof(arg_mode), "0x%x", mode);

	if (oflag & O_CREAT) {
		module_fcall_t call = {
			.type = SP_RTRACE_FTYPE_ALLOC,
			.res_type_id = res_pshmobj.id,
			.name = "shm_open",
			.res_id = (pointer_t)nreg_get_hash(name),
			.res_size = (size_t)1,
		};

		module_farg_t args[] = {
			{.name="name", .value=name},
			{.name="oflag", .value=arg_oflag},
			{.name="mode", .value=arg_mode},
			{.name=NULL, .value=NULL}
		};
		sp_rtrace_write_function_call(&call, NULL, args);
	}
	module_fcall_t call = {
		.type = SP_RTRACE_FTYPE_ALLOC,
		.res_type_id = res_pshmfd.id,
		.name = "shm_open",
		.res_id = (pointer_t)rc,
		.res_size = (size_t)1,
	};

	module_farg_t args[] = {
		{.name="name", .value=name},
		{.name="oflag", .value=arg_oflag},
		{.name="mode", .value=arg_mode},
		{.name=NULL, .value=NULL}
	};
	sp_rtrace_write_function_call(&call, NULL, args);
	return rc;
}


static int trace_shm_unlink(const char *name)
{
	int rc = trace_off.shm_unlink(name);
	if (rc < 0) {
		return rc;
	}
	module_fcall_t call = {
		.type = SP_RTRACE_FTYPE_FREE,
		.res_type_id = res_pshmobj.id,
		.name = "shm_unlink",
		.res_id = (pointer_t)nreg_get_hash(name),
		.res_size = (size_t)0,
	};
	
	module_farg_t args[] = {
		{.name="name", .value=name},
		{.name=NULL, .value=NULL}
	};
	sp_rtrace_write_function_call(&call, NULL, args);
	return rc;
}


static int trace_open(const char *pathname, int flags, ...)
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
	if (rc >= 0) {
		fdreg_store_fd(rc, pathname, FD_FILE, flags);
	}
	return rc;
}


static int trace_open64(const char *pathname, int flags, ...)
{
	int rc = 0;
	if (flags & O_CREAT) {
		va_list args;
		va_start(args, flags);
		rc = trace_off.open64(pathname, flags, va_arg(args, int));
		va_end(args);
	}
	else {
		rc = trace_off.open(pathname, flags);
	}
	backtrace_lock = 0;
	if (rc >= 0) {
		fdreg_store_fd(rc, pathname, FD_FILE, flags);
	}
	return rc;
}


static int trace_creat(const char *pathname, mode_t mode)
{
	int rc = trace_off.creat(pathname, mode);
	if (rc >= 0) {
		fdreg_store_fd(rc, pathname, FD_FILE,  O_CREAT|O_WRONLY|O_TRUNC);
	}
	return rc;
}


static void trace_mmap_common(const char *name, void *rc, size_t length, int prot, int flags, int fd, off64_t offset)
{
	addr_store((pointer_t)rc, fd);
	fdreg_node_t* pfd = fdreg_get_fd(fd);

	module_fcall_t call = {
		.type = SP_RTRACE_FTYPE_ALLOC,
		.res_type_id = (pfd ? (pfd->type == FD_POSIX ? res_pshmmap.id : res_fshmmap.id) : res_shmmap.id),
		.name = name,
		.res_id = (pointer_t)rc,
		.res_size = (size_t)length,
	};

	char arg_length[16], arg_prot[16], arg_flags[16];
	char arg_fd[16], arg_offset[16], arg_mode[16];
	snprintf(arg_length, sizeof(arg_length), "0x%lx", (unsigned long)length);
	snprintf(arg_prot, sizeof(arg_prot), "0x%x", prot);
	snprintf(arg_flags, sizeof(arg_flags), "0x%x", flags);
	module_farg_t args[] = {
		{.name="length", .value=arg_length},
		{.name="prot", .value=arg_prot},
		{.name="flags", .value=arg_flags},
		{.name=NULL, .value=NULL}, // reserved for fd number
		{.name=NULL, .value=NULL}, // reserved for fd offset
		{.name=NULL, .value=NULL}, // reserved for fd name
		{.name=NULL, .value=NULL}, // reserved for fd mode
		{.name=NULL, .value=NULL}
	};
	if (!(flags & (MAP_ANONYMOUS | MAP_ANON))) {
		snprintf(arg_fd, sizeof(arg_fd), "%d", fd);
		snprintf(arg_offset, sizeof(arg_offset), "0x%llx", offset);
		args[3].name = "fd";
		args[3].value = arg_fd;
		args[4].name = "offset";
		args[4].value = arg_offset;
		if (pfd) {
			snprintf(arg_mode, sizeof(arg_mode), "0x%x", pfd->mode);
			args[5].name = "name";
			args[5].value = pfd->name;
			args[6].name = "mode";
			args[6].value = arg_mode;
		}
	}
	sp_rtrace_write_function_call(&call, NULL, args);
}


static void* trace_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	void* rc = trace_off.mmap(addr, length, prot, flags, fd, offset);
	if (rc != (void *) -1) {
		trace_mmap_common("mmap", rc, length, prot, flags, fd, offset);
	}
	return rc;
}

#if TRACE_MMAP2
static void* trace_mmap2(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	void* rc = trace_off.mmap2(addr, length, prot, flags, fd, offset);
	if (rc != (void *) -1) {
		trace_mmap_common("mmap2", rc, length, prot, flags, fd, offset);
	}
	return rc;
}
#endif

static void* trace_mmap64(void *addr, size_t length, int prot, int flags, int fd, off64_t offset)
{
	void* rc = trace_off.mmap64(addr, length, prot, flags, fd, offset);
	if (rc != (void *) -1) {
		trace_mmap_common("mmap64", rc, length, prot, flags, fd, offset);
	}
	return rc;
}


static int trace_munmap(void *addr, size_t length)
{
	int rc = trace_off.munmap(addr, length);
	if (rc < 0) {
		return rc;
	}

	fdreg_node_t* pfd = NULL;
	addr_node_t* paddr = addr_get((pointer_t)addr);
	if (paddr) {
		pfd = fdreg_get_fd(paddr->fd);
	}

	module_fcall_t call = {
		.type = SP_RTRACE_FTYPE_FREE,
		.res_type_id = (pfd ? (pfd->type == FD_POSIX ? res_pshmmap.id : res_fshmmap.id) : res_shmmap.id),
		.name = "munmap",
		.res_id = (pointer_t)addr,
		.res_size = (size_t)0,
	};

	char arg_length[16]; snprintf(arg_length, sizeof(arg_length), "0x%lx", (unsigned long)length);
	module_farg_t args[] = {
		{.name="length", .value=arg_length},
		{.name=NULL, .value=NULL}
	};
	sp_rtrace_write_function_call(&call, NULL, args);
	return rc;
}


static int trace_close(int fd)
{
	int rc = trace_off.close(fd);
	if (rc < 0) {
		return rc;
	}
	backtrace_lock = 0;
	fdreg_node_t* pfd = fdreg_get_fd(fd);
	if (pfd) {
		if (pfd->type == FD_POSIX) {
			module_fcall_t call = {
				.type = SP_RTRACE_FTYPE_FREE,
				.res_type_id = res_pshmfd.id,
				.name = "close",
				.res_id = (pointer_t)fd,
				.res_size = (size_t)0,
			};
			sp_rtrace_write_function_call(&call, NULL, NULL);
		}
#if DO_CLEANUP
		fdreg_remove(fd);
#endif
	}
	return rc;
}


static trace_t trace_on = {
	.shm_open = trace_shm_open,
	.shm_unlink = trace_shm_unlink,
	.open = trace_open,
	.open64 = trace_open64,
	.creat = trace_creat,
	.mmap = trace_mmap,
#if TRACE_MMAP2
	.mmap2 = trace_mmap2,
#endif
	.mmap64 = trace_mmap64,
	.munmap = trace_munmap,
	.close = trace_close,
};


/* target functions */
int shm_open(const char *name, int oflag, mode_t mode)
{
	int rc;
	rc = trace_rt->shm_open(name, oflag, mode);
	return rc;
}


int shm_unlink(const char *name)
{
	int rc;
	rc = trace_rt->shm_unlink(name);
	return rc;
}


int open(const char *pathname, int flags, ...)
{
	int rc;
	if (flags & O_CREAT) {
		va_list args;
		va_start(args, flags);
		int mode = va_arg(args, int);
		va_end(args);
		BT_EXECUTE_LOCKED(rc = trace_rt->open(pathname, flags, mode), trace_off.open(pathname, flags, mode));
	}
	else {
		BT_EXECUTE_LOCKED(rc = trace_rt->open(pathname, flags), trace_off.open(pathname, flags));
	}
	return rc;
}


int open64(const char *pathname, int flags, ...)
{
	int rc;
	if (flags & O_CREAT) {
		va_list args;
		va_start(args, flags);
		int mode = va_arg(args, int);
		va_end(args);
		BT_EXECUTE_LOCKED(rc = trace_rt->open64(pathname, flags, mode), trace_off.open(pathname, flags, mode));
	}
	else {
		BT_EXECUTE_LOCKED(rc = trace_rt->open64(pathname, flags), trace_off.open(pathname, flags));
	}
	return rc;
}


int creat(const char *pathname, mode_t mode)
{
	int rc;
	rc = trace_rt->creat(pathname, mode);
	return rc;
}


void* mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	void* rc;
	rc = trace_rt->mmap(addr, length, prot, flags, fd, offset);
	return rc;
}

#if TRACE_MMAP2
void* mmap2(void *addr, size_t length, int prot, int flags, int fd, off_t pgoffset)
{
	void* rc;
	rc = trace_rt->mmap2(addr, length, prot, flags, fd, pgoffset);
	return rc;
}
#endif

void* mmap64(void *addr, size_t length, int prot, int flags, int fd, off64_t offset)
{
	void* rc;
	rc = trace_rt->mmap64(addr, length, prot, flags, fd, offset);
	return rc;
}


int munmap(void *addr, size_t length)
{
	int rc;
	rc = trace_rt->munmap(addr, length);
	return rc;
}

int close(int fd)
{
	/* synchronize allocation functions used by backtrace */
	int rc;
	BT_EXECUTE_LOCKED(rc = trace_rt->close(fd), trace_off.close(fd));
	return rc;
}


/*
 * Initialization functions.
 */
static int init_shm_open(const char *name, int oflag, mode_t mode)
{
	trace_initialize();
	int rc = trace_init_rt->shm_open(name, oflag, mode);
	return rc;
}

static int init_shm_unlink(const char *name)
{
	trace_initialize();
	int rc = trace_init_rt->shm_unlink(name);
	return rc;
}

static int init_open(const char *pathname, int flags, ...)
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

static int init_open64(const char *pathname, int flags, ...)
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

static int init_creat(const char *pathname, mode_t mode)
{
	trace_initialize();
	int rc = trace_init_rt->creat(pathname, mode);
	return rc;
}

static void* init_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
	trace_initialize();
	void* rc = trace_init_rt->mmap(addr, length, prot, flags, fd, offset);
	return rc;
}

#if TRACE_MMAP2
static void* init_mmap2(void *addr, size_t length, int prot, int flags, int fd, off_t pgoffset)
{
	trace_initialize();
	void* rc = trace_init_rt->mmap2(addr, length, prot, flags, fd, pgoffset);
	return rc;
}
#endif

static void* init_mmap64(void *addr, size_t length, int prot, int flags, int fd, off64_t offset)
{
	trace_initialize();
	void* rc = trace_init_rt->mmap64(addr, length, prot, flags, fd, offset);
	return rc;
}

static int init_munmap(void *addr, size_t length)
{
	trace_initialize();
	int rc = trace_init_rt->munmap(addr, length);
	return rc;
}

static int init_close(int fd)
{
	trace_initialize();
	return trace_init_rt->close(fd);
}

static trace_t trace_init = {
	.shm_open = init_shm_open,
	.shm_unlink = init_shm_unlink,
	.open = init_open,
	.open64 = init_open64,
	.creat = init_creat,
	.mmap = init_mmap,
#if TRACE_MMAP2
	.mmap2 = init_mmap2,
#endif
	.mmap64 = init_mmap64,
	.munmap = init_munmap,
	.close = init_close,
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
