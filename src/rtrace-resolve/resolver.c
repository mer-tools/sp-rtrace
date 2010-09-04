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
#include <stdlib.h>
#include <stdio.h>
#include <elf.h>
#include <bfd.h>
#include <errno.h>
#include <stdbool.h>
#include <limits.h>

#include "resolver.h"
#include "common/utils.h"
#include "namecache.h"

#define DMGL_PARAMS  (1 << 0) /* Include function args */
#define DMGL_ANSI  (1 << 1)   /* Include const, volatile, etc */

extern char* cplus_demangle(const char* symbol, int flags);

#define TARGET    "default"

#define RS_MMAPS_INDEX_SIZE       256

/* 64 bit compatibility */

#ifdef __amd64__
 typedef Elf64_Ehdr Elf_Ehdr;
 typedef Elf64_Phdr Elf_Phdr;
#else
 typedef Elf32_Ehdr Elf_Ehdr;
 typedef Elf32_Phdr Elf_Phdr;
#endif


 /**
  * Free resolver cache record.
  *
  * @param[in] rec    the cache record to free.
  */
 static void rs_cache_record_clear(rs_cache_record_t* rec)
 {
 	if (rec->symbols) {
 		free(rec->symbols);
 		rec->symbols = NULL;
 	}
 	if (rec->file) {
 		bfd_close(rec->file);
 		rec->file = NULL;
 	}
 	rec->mmap = NULL;
 }

/*
 * Memory mapping support
 */

/**
 *
 * @param path
 * @return
 */
static int rs_mmap_is_absolute(const char* path)
{
	FILE *file;
	Elf_Ehdr elf_header;
	Elf_Phdr *program_header;
	size_t ret;
	int i, is_absolute = 1;

	if (!(file = fopen(path, "r"))) {
//		fprintf(stderr, "WARNING: could not open file %s\n", path);
		return -ENOENT;
	}

	/* read ELF header */
	ret = fread(&elf_header, sizeof(elf_header), 1, file);
	if (ret != 1) {
		fprintf(stderr, "ERROR: invalid ELF header from %s\n", path);
		return -EINVAL;
	}

	program_header = (Elf_Phdr *)malloc_a(sizeof(Elf_Phdr) * elf_header.e_phnum);

	fseek(file, elf_header.e_phoff, SEEK_SET);

	/* read program header table */
	ret = fread(program_header, sizeof(Elf_Phdr), elf_header.e_phnum, file);
	if (ret != elf_header.e_phnum) {
		free(program_header);
		fprintf(stderr, "ERROR: could not read program header table from %s\n", path);
		return -EINVAL;
	}

	for (i = 0; i < elf_header.e_phnum; i++) {
		if ((program_header[i].p_type == PT_LOAD) && (!program_header[i].p_offset)) {
			if (!program_header[i].p_vaddr) {
				is_absolute = 0;
			}
			break;
		}
	}

	free(program_header);
	fclose(file);

	return is_absolute;
}

/**
 * Memory mapping comparison function.
 *
 * @param mmap1
 * @param mmap2
 * @return
 */
static long mmap_compare(const rs_mmap_t* mmap1, const rs_mmap_t* mmap2)
{
	/* first check if the specified from address is inside mmap2 region.
	 * (used for address->module lookups) */
	if (mmap1->from >= mmap2->from && mmap1->from <= mmap2->to) return 0;
	/* then simply return from address comparison */
	return mmap1->from - mmap2->from;
}

/**
 * Frees memory mapping record
 *
 * @param[in] mmap
 * @return
 */
static void rs_mmap_free_node(rs_mmap_t* mmap)
{
	if (mmap->module) free(mmap->module);
	if (mmap->is_cache_owner) {
		rs_cache_record_clear(mmap->cache);
		free(mmap->cache);
	}
	free(mmap);
}


/*
 * Resolving logic
 */

/**
 * Opens the specified file.
 *
 * @param[in] rec  the resolver cache record.
 * @param[in] filename   the file name.
 * @return               0 - success.
 */
static int rs_open_file(rs_cache_record_t* rec, const char* filename)
{
	rec->file = bfd_openr(filename, TARGET);

	if (rec->file == NULL) {
		fprintf(stderr, "ERROR: %s: %s\n", filename, bfd_errmsg(bfd_get_error()));
		return -EINVAL;
	}

	if (!bfd_check_format (rec->file, bfd_object)) {
		bfd_close(rec->file);
		rec->file = NULL;
		fprintf(stderr, "error: file %s not in executable format\n", filename);
		return -EINVAL;
	}
	return 0;
}



/**
 * Read symbol table.
 *
 * @param[in] rec       the resolver cache record.
 * @param[in] filename  the file to read from.
 * @return
 */
static int rs_load_symbols(rs_cache_record_t* rec, const char* filename)
{
	unsigned int size;
	static char *places[]={".","./.debug", "/usr/lib/debug", NULL};

	/* locate and open the symbol file */
	if (rs_open_file(rec, filename) < 0) return -EINVAL;

	int index = 0;
	while (places[index]) {
		char* dbg_name = bfd_follow_gnu_debuglink (rec->file, places[index]);
		if (dbg_name) {
			bfd_close(rec->file);
			int rc = rs_open_file(rec, dbg_name);
			free(dbg_name);
			if (rc < 0) return -EINVAL;
			break;
		}
		index++;
	}
	/* read the symbol table from opened file */
	if ((bfd_get_file_flags (rec->file) & HAS_SYMS) == 0) {
		fprintf(stderr, "ERROR: no symbols in %s\n", bfd_get_filename(rec->file));
		return -EINVAL;
	}

	long symcount = bfd_read_minisymbols(rec->file, FALSE, (void *) &rec->symbols, &size);
	if (symcount == 0) {
		symcount = bfd_read_minisymbols(rec->file, TRUE , (void *) &rec->symbols, &size);
	}

	if (symcount < 0) {
		fprintf(stderr, "ERROR: %s: %s\n", bfd_get_filename(rec->file), bfd_errmsg(bfd_get_error()));
		return -EINVAL;
	}
	rec->symcount = symcount;
	return 0;
}

/**
 * Retrieves address information (function name, file, line number).
 *
 * @param[in] bfd_cache  the bfd cache data.
 * @param[in] address    the address to look up.
 * @param[out] buffer    the output buffer.
 * @return               0 - success.
 */
static int rs_get_address_info(rs_cache_record_t* rec, pointer_t address, char* buffer)
{
	const char *filename, *functionname;
	unsigned int line;
	void* abs_address = (void*)address;

	asection *section;
	bfd_vma pc, vma;
	bfd_size_type size;

	if (!rec->mmap->is_absolute) {
		abs_address -= (unsigned long)rec->mmap->from;
	}
	char addr_hex[LINE_MAX];
	sprintf(addr_hex, "%#lx", (long)abs_address);

	pc = bfd_scan_vma(addr_hex, NULL, 16);

	for (section = rec->file->sections; section != NULL; section = section->next) {

		if ((bfd_get_section_flags(resolver->bfd_file, section) & SEC_ALLOC) == 0)
			continue;

		vma = bfd_get_section_vma(resolver->bfd_file, section);
		if (pc < vma)
			continue;

		size = bfd_get_section_size(section);
		if (pc >= vma + size)
			continue;

		if (bfd_find_nearest_line(rec->file, section, rec->symbols, pc - vma, &filename, &functionname, &line)) {
			char* ptr_out = buffer;
			ptr_out += sprintf(ptr_out, "\t0x%lx (", address);
			if (functionname) {
				ptr_out += sprintf(ptr_out, "in ");
				char *alloc;

				if (functionname[0] == 'I' && functionname[1] == 'A' &&
					functionname[2] == '_' && functionname[3] == '_')
					functionname = functionname + 4;

				alloc = (char*)cplus_demangle(functionname, DMGL_ANSI | DMGL_PARAMS);
				if (alloc != NULL) {
					ptr_out += sprintf(ptr_out, "%s", alloc);
					free(alloc);
				} else {
					ptr_out += sprintf(ptr_out, "%s", functionname);
				}
			}

			if (filename) {
				ptr_out += sprintf(ptr_out, " at %s", filename);
				if (line) ptr_out += sprintf(ptr_out, ":%d", line);
			}
			ptr_out += sprintf(ptr_out, ")\n");
			return 0;
		}
	}

	return -EINVAL;
}


/*
 * Public API
 */

const char* rs_resolve_address(rs_cache_t* rs, pointer_t address)
{
	static char buffer[PATH_MAX];

	namecache_t* nc = namecache_get_data(address);
	if (nc) return nc->name;

	while (true) {
		rs_mmap_t* mmap = rs_mmap_find_module(&rs->mmaps, address);
		if (mmap == NULL) {
			sprintf(buffer, "\t0x%lx <unknown>\n", address);
			break;
		}
		if (mmap != mmap->cache->mmap) {
			rs_cache_record_clear(mmap->cache);
			if (rs_load_symbols(mmap->cache, mmap->module) < 0) {
				rs_cache_record_clear(mmap->cache);
				sprintf(buffer, "\t0x%lx (%s)\n", address, mmap->module);
				break;
			}
			mmap->cache->mmap = mmap;
		}
		if (rs_get_address_info(mmap->cache, address, buffer) < 0) {
			sprintf(buffer, "\t0x%lx (%s)\n", address, mmap->module);
			break;
		}
		break;
	}
	namecache_add(address, buffer);
	return buffer;
}

rs_mmap_t* rs_mmap_add_module(rs_cache_t* rs, const char* module, pointer_t from, pointer_t to, bool single_cache)
{
	rs_mmap_t* mmap = NULL;
	int is_absolute = rs_mmap_is_absolute(module);
	if (is_absolute >= 0) {
		mmap = malloc_a(sizeof(rs_mmap_t));
		mmap->module = strdup_a(module);
		mmap->from = from;
		mmap->to = to;
		mmap->fin = NULL;
		mmap->fout = NULL;
		mmap->is_absolute = is_absolute;
		mmap->id = rs->mmaps_size;
		mmap->cache = single_cache ?  &rs->cache : (rs_cache_record_t*)calloc_a(1, sizeof(rs_cache_record_t));
		mmap->is_cache_owner = !single_cache;
		sarray_add(&rs->mmaps, mmap);
		if (rs->mmaps_size == rs->mmaps_limit) {
			rs->mmaps_limit *= 2;
			rs->mmaps_index = (rs_mmap_t**)realloc_a(rs->mmaps_index, rs->mmaps_limit * sizeof(rs_mmap_t*));
		}
		rs->mmaps_index[rs->mmaps_size++] = mmap;
		return mmap;
	}
	return NULL;
}

rs_mmap_t* rs_mmap_find_module(sarray_t* mmaps, pointer_t addr)
{
	/* mmap_compare function uses only .from field to compare records.
	 * Setting it to specified address is enough to locate the responsible
	 * mmap record */
	rs_mmap_t template = {.from = addr};
	return sarray_find(mmaps, &template);
}

void rs_cache_init(rs_cache_t* rs)
{
	memset(&rs->cache, 0, sizeof(rs_cache_record_t));
	sarray_init(&rs->mmaps, RS_MMAPS_INDEX_SIZE, (op_binary_t)mmap_compare);
	rs->mmaps_index = (rs_mmap_t**)malloc_a(RS_MMAPS_INDEX_SIZE * sizeof(rs_mmap_t*));
	rs->mmaps_index[0] = NULL;
	rs->mmaps_limit = RS_MMAPS_INDEX_SIZE;
	rs->mmaps_size = 1;
}

void rs_cache_free(rs_cache_t* rs)
{
	rs_cache_record_clear(&rs->cache);
	sarray_free(&rs->mmaps, (op_unary_t)rs_mmap_free_node);

	free(rs->mmaps_index);
}


