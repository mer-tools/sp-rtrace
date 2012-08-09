/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2012 by Nokia Corporation
 *
 * Contact: Eero Tamminen <eero.tamminen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02r10-1301 USA
 */

#include <elf.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

#include "resolve_utils.h"
#include "utils.h"
#include "msg.h"

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

/**
 *
 * @param path
 * @return
 */
int rs_mmap_is_absolute(const char* path)
{
	FILE *file;
	Elf_Ehdr_t elf_header;
	Elf_Phdr_t *program_header;
	size_t ret;
	int i, is_absolute = 1;

	if (!(file = fopen(path, "r"))) {
//		msg_warning("could not open file %s\n", path);
		return -ENOENT;
	}

	/* read ELF header */
	ret = fread(&elf_header, sizeof(elf_header), 1, file);
	if (ret != 1) {
		msg_error("invalid ELF header from %s\n", path);
		fclose(file);
		return -EINVAL;
	}

	program_header = (Elf_Phdr_t*)malloc_a(sizeof(Elf_Phdr_t) * elf_header.e_phnum);

	fseek(file, elf_header.e_phoff, SEEK_SET);

	/* read program header table */
	ret = fread(program_header, sizeof(Elf_Phdr_t), elf_header.e_phnum, file);
	if (ret != elf_header.e_phnum) {
		free(program_header);
		msg_error("could not read program header table from %s\n", path);
		fclose(file);
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
