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
#include <stdlib.h>
#include <stdio.h>
#include <elf.h>
#include <bfd.h>
#include <errno.h>
#include <stdbool.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <demangle.h>

#include "resolver.h"
#include "sp_rtrace_resolve.h"
#include "common/resolve_utils.h"
#include "common/utils.h"
#include "common/msg.h"
#include "namecache.h"

#define DMGL_PARAMS  (1 << 0) /* Include function args */
#define DMGL_ANSI  (1 << 1)   /* Include const, volatile, etc */

#define TARGET    "default"

#define RS_MMAPS_INDEX_SIZE       256

/* the name used for unknown symbols */
#define UNKNOWN_SYMBOL    "in ??"

/* same as in bfd header except for bogus (char*) cast */
#define bfd_get_const_filename(abfd) (abfd)->filename

/* 64 bit compatibility */

#ifndef __amd64__
	typedef Elf32_Ehdr   Elf_Ehdr_t;
	typedef Elf32_Phdr   Elf_Phdr_t;
	typedef Elf32_Sym    Elf_Sym_t;
	typedef Elf32_Off    Elf_Off_t;
	typedef Elf32_Shdr   Elf_Shdr_t;
	typedef Elf32_Addr   Elf_Addr_t;

	#define ELF_ST_TYPE(x)		ELF32_ST_TYPE(x)
#else
	typedef Elf64_Ehdr   Elf_Ehdr_t;
	typedef Elf64_Phdr   Elf_Phdr_t;
	typedef Elf64_Sym    Elf_Sym_t;
	typedef Elf64_Off    Elf_Off_t;
	typedef Elf64_Shdr   Elf_Shdr_t;
	typedef Elf64_Addr   Elf_Addr_t;

	#define ELF_ST_TYPE(x)		ELF64_ST_TYPE(x)

#endif

typedef struct {
	const char* name;
	const char* source;
	unsigned int line;
} symbol_info_t;

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
 	if (rec->dbg_name) {
 		free(rec->dbg_name);
 		rec->dbg_name = NULL;
 	}
 	if (rec->fd) {
 		munmap(rec->image, rec->image_size);
 		close(rec->fd);
 		rec->fd = 0;
 		rec->image = NULL;
 	}
 	rec->mmap = NULL;
 }

/*
 * Memory mapping support
 */


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
static void rs_mmap_free_node(rs_mmap_t* map)
{
	if (map->module) free(map->module);
	if (map->is_cache_owner) {
		rs_cache_record_clear(map->cache);
		free(map->cache);
	}
	free(map);
}


/*
 * Resolving logic
 */


/**
 * Retrieves address information (function name, file, line number) through
 * bfd interface.
 *
 * @param[in] rec        the file cache record.
 * @param[in] address    the address to look up.
 * @param[out] symbol     the output buffer.
 * @return               0 - success.
 */
static int elf_get_address_info(rs_cache_record_t* rec, pointer_t address, symbol_info_t* symbol)
{
	Elf_Ehdr_t *ehdr = rec->image;
	Elf_Sym_t *sym, *symtab, *symtab_end;
	Elf_Off_t soff = ehdr->e_shoff, str_soff;
	Elf_Shdr_t *shdr, *str_shdr;

	pointer_t abs_address = address;
	if (!rec->mmap->is_absolute) {
		abs_address -= rec->mmap->from;
	}

	shdr = (Elf_Shdr_t*)((char*)rec->image + soff);

	if (soff + ehdr->e_shnum * ehdr->e_shentsize > rec->image_size) {
		return -EINVAL;
	}

	int i;
	for (i = 0; i < ehdr->e_shnum; ++i) {
		switch (shdr->sh_type) {
			case SHT_SYMTAB:
			case SHT_DYNSYM: {
				symtab = (Elf_Sym_t*)((char*)rec->image + shdr->sh_offset);
				symtab_end = (Elf_Sym_t*)((char*)symtab + shdr->sh_size);
				size_t symtab_size = shdr->sh_entsize;

				str_soff = soff + (shdr->sh_link * ehdr->e_shentsize);
				if (str_soff + ehdr->e_shentsize >= rec->image_size) break;

				str_shdr = (Elf_Shdr_t*)((char*)rec->image + str_soff);
				char* strtab = (char*)rec->image + str_shdr->sh_offset;

				for (sym = symtab; sym < symtab_end; sym = (Elf_Sym_t*)((char*)sym + symtab_size)) {
					if (ELF_ST_TYPE(sym->st_info) == STT_FUNC && sym->st_shndx != SHN_UNDEF) {
						if ((Elf_Addr_t)abs_address >= sym->st_value && (abs_address - sym->st_value) < sym->st_size) {
							if (strtab + sym->st_name) {
								symbol->name =  strtab + sym->st_name;
								return 0;
							}
							return -EINVAL;
						}
					}
				}
				break;
			}

			default:
				break;
		}
		shdr = (Elf_Shdr_t*)(((char *)shdr) + ehdr->e_shentsize);
	}
	return -EINVAL;
}


/**
 * Retrieves address information (function name, file, line number) through
 * bfd interface.
 *
 * @param[in] rec        the bfd cache data.
 * @param[in] address    the address to look up.
 * @param[out] symbol    the output buffer.
 * @return               0 - success.
 */
static int bfd_get_address_info(rs_cache_record_t* rec, pointer_t address, symbol_info_t* symbol)
{
	pointer_t abs_address = address;

	asection *section;
	bfd_vma pc, vma;
	bfd_size_type size;

	if (!rec->mmap->is_absolute) {
		abs_address -= rec->mmap->from;
	}

	char addr_hex[LINE_MAX];
	sprintf(addr_hex, "%#lx", abs_address);

	/* If frame is not the innermost frame, that normally means that
	   pc points to *after* the call instruction, and we want to
	   get the line containing the call, never the next line. */
	pc = bfd_scan_vma(addr_hex, NULL, 16) - 1;

	for (section = rec->file->sections; section != NULL; section = section->next) {

		if ((bfd_get_section_flags(resolver->bfd_file, section) & SEC_ALLOC) == 0)
			continue;

		vma = bfd_get_section_vma(resolver->bfd_file, section);
		if (pc < vma)
			continue;

		size = bfd_get_section_size(section);
		if (pc >= vma + size)
			continue;

		if (bfd_find_nearest_line(rec->file, section, rec->symbols, pc - vma, &symbol->source, &symbol->name, &symbol->line)) {
			return 0;
		}
	}

	return -EINVAL;
}


/**
 * Retrieves address information (function name, file, line number).
 *
 * @param[in] rec        the file cache record.
 * @param[in] address    the address to look up.
 * @param[out] buffer    the output buffer.
 * @return               0 - success.
 */
static int get_address_info(rs_cache_record_t* rec, pointer_t address, char* buffer)
{
	symbol_info_t bfd = {.name=NULL, .source=NULL};
	symbol_info_t elf = {.name=NULL, .source=NULL};
	symbol_info_t* sym = NULL;
	char* ptr_out = buffer;

	if (resolve_options.mode & MODE_BFD) {
		int rc = bfd_get_address_info(rec, address, &bfd);
		if ( !(resolve_options.mode & MODE_ELF) && rc == 0) {
			sym = &bfd;
		}
	}
	if (resolve_options.mode & MODE_ELF) {
		int rc = elf_get_address_info(rec, address, &elf);
		if ( !(resolve_options.mode & MODE_BFD) && rc == 0) {
			sym = &elf;
		}
	}
	if ( (resolve_options.mode & MODE_METHOD_MASK) == (MODE_ELF | MODE_BFD) ) {
		if (!elf.name) {
			sym = NULL;
		}
		else if (!bfd.name || strcmp(elf.name, bfd.name)) {
			sym = &elf;
		}
		else {
			sym = &bfd;
		}
	}
	if (sym == NULL) return -EINVAL;

	ptr_out += sprintf(ptr_out, "\t0x%lx ", address);
	if (sym->name) {
		char *alloc;
		const char* functionname = sym->name;

		if (functionname[0] == 'I' && functionname[1] == 'A' &&
			functionname[2] == '_' && functionname[3] == '_')
			functionname = functionname + 4;

		alloc = (char*)cplus_demangle(functionname, DMGL_ANSI | DMGL_PARAMS);
		if (alloc != NULL) {
			ptr_out += sprintf(ptr_out, "%s", alloc);
			free(alloc);
		} else {
			ptr_out += sprintf(ptr_out, "%s()", functionname);
		}
	}
	if (sym->source) {
		/* strip the path if not told otherwise */
		const char* source = sym->source;
		if (!resolve_options.full_path) {
			source = strrchr(sym->source, '/');
			if (source) source++;
			else source = sym->source;
		}
		ptr_out += sprintf(ptr_out, " at %s", source);
		if (sym->line) ptr_out += sprintf(ptr_out, ":%d", sym->line);
	}
	else {
		ptr_out += sprintf(ptr_out, " from %s", rec->mmap->module);
	}
	*ptr_out++ = '\n';
	*ptr_out = '\0';
	return 0;
}

/**
 * Opens the specified file.
 *
 * @param[in] rec  the resolver cache record.
 * @param[in] filename   the file name.
 * @return               0 - success.
 */
static int rs_open_file(rs_cache_record_t* rec, const char* filename)
{
	rec->file = bfd_openr(rs_host_path(filename), TARGET);

	if (rec->file == NULL) {
		msg_error("%s: %s\n", rs_host_path(filename), bfd_errmsg(bfd_get_error()));
		return -EINVAL;
	}

	if (!bfd_check_format (rec->file, bfd_object)) {
		bfd_close(rec->file);
		rec->file = NULL;
		msg_error("file %s not in executable format\n", rs_host_path(filename));
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
	long symcount = 0;
	unsigned int size;
	if (resolve_options.mode & MODE_BFD) {
		const char *places[] = {".", "./.debug", "/usr/lib/debug", NULL};

		/* locate and open the symbol file */
		if (rs_open_file(rec, filename) < 0) return -EINVAL;

		int idx = 0;
		while (places[idx]) {
			rec->dbg_name = bfd_follow_gnu_debuglink (rec->file, rs_host_path(places[idx]));
			if (rec->dbg_name) {
				bfd_close(rec->file);
				int rc = rs_open_file(rec, rec->dbg_name);
				if (rc < 0) return -EINVAL;
				break;
			}
			idx++;
		}
		/* read the symbol table from opened file */
		if ((bfd_get_file_flags (rec->file) & HAS_SYMS) == 0) {
			msg_error("no symbols in %s\n", bfd_get_const_filename(rec->file));
			return -EINVAL;
		}

		symcount = bfd_read_minisymbols(rec->file, FALSE, (void *) &rec->symbols, &size);
	}
	if (symcount == 0 && !(resolve_options.mode & MODE_ELF)) {
		if (rec->symbols) free(rec->symbols);
		symcount = bfd_read_minisymbols(rec->file, TRUE, (void *) &rec->symbols, &size);
	}
	if (resolve_options.mode & MODE_ELF) {
		/* Use elf symbol table lookup, as the bfd_find_nearest_line returns bogus information
		 * for global constructors when dynamic symbol tables are used.
		 */
		rec->fd = open(rs_host_path(filename), O_RDONLY);
		if (rec->fd == -1) {
			fprintf(stderr, "Failed to open image file %s\n", rs_host_path(filename));
			return -EINVAL;
		}
		struct stat sb;
		if (fstat(rec->fd, &sb) < 0 || sb.st_size <= EI_CLASS) {
			fprintf(stderr, "Image file too short to contain elf header\n");
			close(rec->fd);
			return -EINVAL;
		}

		rec->image_size = sb.st_size;

		rec->image = mmap (NULL, rec->image_size, PROT_READ, MAP_PRIVATE, rec->fd, 0);
		if (rec->image == MAP_FAILED) {
			fprintf(stderr, "Failed to map elf image\n");
			close(rec->fd);
			return -EINVAL;
		}
		if (memcmp(rec->image, ELFMAG, SELFMAG) != 0) {
			fprintf(stderr, "Elf header identification failed\n");
			return -EINVAL;
		}
		symcount = 0;
	}

	if (symcount < 0) {
		msg_error("%s: %s\n", bfd_get_const_filename(rec->file), bfd_errmsg(bfd_get_error()));
		return -EINVAL;
	}
	rec->symcount = symcount;

	return 0;
}

/*
 * Public API
 */

const char* rs_resolve_address(rs_cache_t* rs, pointer_t address, const char* name)
{
	static char buffer[PATH_MAX];

	if (*name && resolve_options.keep_resolved) {
		/* The input stream had the address resolved and the option to keep resolved
		 * addresses is set. Return the old address (stripping the source path if necessary). */
		const char* source;
		if (!resolve_options.full_path && (source = strrchr(name, '/')) != NULL ) {
			const char* path = strrchr(source, ' ');
			char* ptr = buffer + sprintf(buffer, "\t0x%lx ", address);
			int len = path - name;
			memcpy(ptr, name, len);
			sprintf(ptr + len, "%s", source);
		}
		else {
			sprintf(buffer, "\t0x%lx %s\n", address, name);
		}
		return buffer;
	}

	/* process with address resolving */
	namecache_t* nc = namecache_get_data(address);
	if (nc) return nc->name;

	while (true) {
		rs_mmap_t* map = rs_mmap_find_module(&rs->mmaps, address);
		if (map == NULL) {
			sprintf(buffer, "\t0x%lx %s\n", address, UNKNOWN_SYMBOL);
			break;
		}
		if (map != map->cache->mmap) {
			rs_cache_record_clear(map->cache);
			if (rs_load_symbols(map->cache, map->module) < 0) {
				rs_cache_record_clear(map->cache);
				sprintf(buffer, "\t0x%lx from %s\n", address, map->module);
				break;
			}
			map->cache->mmap = map;
		}
		if (get_address_info(map->cache, address, buffer) < 0) {
			sprintf(buffer, "\t0x%lx from %s\n", address, map->module);
			break;
		}
		break;
	}
	namecache_add(address, buffer);
	return buffer;
}

rs_mmap_t* rs_mmap_add_module(rs_cache_t* rs, const char* module, pointer_t from, pointer_t to, bool single_cache)
{
	rs_mmap_t* map = NULL;
	int is_absolute = rs_mmap_is_absolute(module);
	if (is_absolute >= 0) {
		map = malloc_a(sizeof(rs_mmap_t));
		map->module = strdup_a(module);
		map->from = from;
		map->to = to;
		map->fin = NULL;
		map->fout = NULL;
		map->is_absolute = is_absolute;
		map->id = rs->mmaps_size;
		map->cache = single_cache ?  &rs->cache : (rs_cache_record_t*)calloc_a(1, sizeof(rs_cache_record_t));
		map->is_cache_owner = !single_cache;
		sarray_add(&rs->mmaps, map);
		if (rs->mmaps_size == rs->mmaps_limit) {
			rs->mmaps_limit *= 2;
			rs->mmaps_index = (rs_mmap_t**)realloc_a(rs->mmaps_index, rs->mmaps_limit * sizeof(rs_mmap_t*));
		}
		rs->mmaps_index[rs->mmaps_size++] = map;
		return map;
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


const char* rs_host_path(const char* path)
{
	if (!resolve_options.root_path || *path != '/') return path;
	static char host_path[PATH_MAX];
	snprintf(host_path, PATH_MAX, "%s/%s", resolve_options.root_path, path);
	return host_path;
}
