/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2011 by Nokia Corporation
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
 * @file sp_rtrace_pagemap.c
 *
 * Module to trace empty pagemap pages ((libsp-rtrace-pagemap.so).
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#include "sp_rtrace_main.h"
#include "sp_rtrace_module.h"
#include "common/sp_rtrace_proto.h"
#include "common/sp_rtrace_defs.h"
#include "common/debug_log.h"
#include "common/utils.h"


//#define MSG(text) {	char buffer[] = ">>>" text "\n"; if(write(STDERR_FILENO, buffer, sizeof(buffer))){}; }

/* Module information */
static sp_rtrace_module_info_t module_info = {
		.type = MODULE_TYPE_PRELOAD,
		.version_major = 1,
		.version_minor = 0,
		.name = "pagemap",
		.description = "Empty pagemap pages tracking module. "
				       "Tracks allocated pages containing only zero bytes and attaches page mapping data "
				       "from /proc/pid/pagemap and /proc/pageflags.",
};

static bool trace_enabled = false;


/**
 * The memory page data.
 *
 * This structure is used to store data about memory pages containing
 * only zeroes. It holds starting page address and the number of
 * consequent pages containing only zero bytes.
 */
typedef struct {
	/* starting area */
	unsigned long addr;
	/* number of pages */
	unsigned long npages;
} pagescan_t;


static unsigned long page_size;

/**
 * Locates character in string.
 *
 * @param[in] str   the source string.
 * @param[in] c     the character to locate.
 * @param[in] size  the size of the input string.
 * @return          a reference to the located character or NULL
 *                  if the character was not found.
 */
static char* _strnchr(char* str, int c, size_t size)
{
	char* start = str;
	while (*str != c) {
		if (*str == '\0' || (size_t)(str - start) >= size ) return NULL;
		str++;
	}
	return str;
}

/**
 * Copies memory area.
 *
 * @param[in] dest  the destination area.
 * @param[in] src   the source area.
 * @param[in] size  the number of bytes to copy.
 */
static void _strmove(char *dest, const char *src, size_t size)
{
	char* start;
	for (start = dest; (size_t)(dest - start) < size; dest++, src++) {
		*dest = *src;
	}
}

/**
 * Converts reads hexadecimal value from string format.
 *
 * @param[in] str   the input string containing hexadecimal value.
 * @return          the scanned value.
 */
static unsigned long str2hex(const char* str)
{
	unsigned long value = 0;
	while (*str) {
		int digit = (unsigned char)*str++;
		if (digit >= 'a') digit -= 'a' - 10;
		else if (digit >= 'A') digit -= 'A' - 10;
		else digit -= '0';
		value = (value << 4) | digit;
	}
	return value;
}

/**
 * Checks if the memory page is filled with zeroes.
 *
 * @param[in] from   the start address.
 * @return           true if the page contains only zero bytes, false otherwise.
 */
static bool is_zero_page(unsigned long from)
{
	unsigned long* ptr = (unsigned long*)from;
	while ((unsigned long)ptr - from < page_size) {
		if (*ptr != 0) return false;
		ptr++;
	}
	return true;
}

/*
static void print_pdata(pagescan_t* pdata, int npages)
{
	int i;
	for (i = 0; i < npages; i++) {
		printf("[map] 0x%lx, %lu\n", pdata->addr, pdata->npages);
		pdata++;
	}
}
*/

/**
 * Scans address range for memory pages containing zeroes.
 *
 * The data is stored in pagescan_t structure array and written
 * in the specified file.
 * @param[in] fd_out  the output file descriptor.
 * @param[in] from    the memory area start address.
 * @param[in] to      the memory area end address.
 * @param[in] module  the module(file) mapped to the specified address range.
 * @return             0 - success.
 *                    <0 - -errno error code.
 */
static int scan_address_range(int fd_out, unsigned long from, unsigned long to, const char* module)
{
	pagescan_t data[512], *pdata = data;

	pdata->npages = 0;
	pdata->addr = 0;
	while (from < to) {
		if (is_zero_page(from)) {
			pdata->addr = from;
			pdata->npages++;
		}
		else {
			if (pdata->npages) {
				pdata++;
				if ((unsigned long)(pdata - data) >= sizeof(data) / sizeof(data[0])) {
					long size = (pdata - data) * sizeof(data[0]);
					if (write(fd_out, data, size) != size) return -errno;
					pdata = data;
				}
				pdata->npages = 0;
			}
		}
		from += page_size;
	}
	if (pdata->npages) pdata++;
	if (pdata > data) {
		long size = (pdata - data) * sizeof(data[0]);
		if (write(fd_out, data, size) != size) return -errno;
	}
	return 0;
}

/**
 * Parses /proc/pid/maps file record (line).
 *
 * This function parses the maps file record and scans the r/w
 * address range for memory pages containing only zero bytes.
 * @param[in] fd_out  the output file descriptor.
 * @param[in] buffer  the maps file record. The data in this
 *                    buffer is changed during parsing.
 * @param[in] size    the size of maps file record.
 * @return             0 - success.
 *                    <0 - -errno error code.
 */
static int parse_record(int fd_out, char* buffer, size_t size)
{
	char* from_s = buffer;
	char *ptr = _strnchr(buffer, '-', size);
	if (!ptr) return -EINVAL;
	*ptr++ = '\0';

	char* to_s = ptr;
	ptr = _strnchr(to_s, ' ', size - (to_s - buffer));
	if (!ptr) return -EINVAL;
	*ptr++ = '\0';

	char* rights_s = ptr;
	ptr = _strnchr(rights_s, ' ', size - (rights_s - buffer));
	if (!ptr) return -EINVAL;
	if (rights_s[0] != 'r' || rights_s[1] != 'w') return -EINVAL;
	*ptr++ = '\0';

	ptr = _strnchr(ptr, ' ', size - (ptr - buffer));
	if (!ptr) return -EINVAL;
	ptr++;

	ptr = _strnchr(ptr, ' ', size - (ptr - buffer));
	if (!ptr) return -EINVAL;
	ptr++;

	char* name_s = _strnchr(ptr, ' ', size - (ptr - buffer));
	if (name_s) {
		while (*name_s == ' ') name_s++;
	}
	else {
		name_s = buffer + size - 1;
	}

	unsigned long from = str2hex(from_s);
	unsigned long to = str2hex(to_s);

	return scan_address_range(fd_out, from, to, name_s);
}

/**
 * Parses file buffer for lines.
 *
 * The buffer might contain several lines from /proc/pid/maps.
 * This function separates them and call parse_record() function
 * for each line.
 * @param[in] fd_out      the output file descriptor.
 * @param[in] buffer      the buffer to parse.
 * @param[in] total_size  the number of bytes in buffer.
 * @return                 0 - success.
 *                        <0 - -errno error code.
 */
static int parse_buffer(int fd_out, char* buffer, size_t total_size)
{
	buffer[total_size] = '\0';;
	char* end = buffer;
	while (true) {
		char* ptr = _strnchr(end, '\n', total_size - (end - buffer));
		if (!ptr) break;
		*ptr++ = '\0';
		parse_record(fd_out, end, ptr - end);
		end = ptr;
	}
	return end - buffer;
}

/**
 * Parses /proc/self/maps file.
 *
 * @param[in] fd_out   the output file descriptor.
 * @return              0 - success.
 *                     <0 - -errno error code.
 */
static int parse_maps(int fd_out)
{
	int fd = open("/proc/self/maps", O_RDONLY);
	if (fd == -1) return -errno;

	char buffer[4096];
	int n, offset = 0;
	while ( (n = read(fd, buffer + offset, sizeof(buffer) - offset)) ) {
		size_t total_size = n + offset;
		int nparsed = parse_buffer(fd_out, buffer, total_size);
		_strmove(buffer, buffer + nparsed, total_size - nparsed);
	}
	close(fd);
	return 0;
}


/**
 * Scans the current process address map for memory pages containing
 * only zeroes.
 *
 * The data is written in the specified file in binary format. See
 * the pagescan_t data structure.
 * @param[in] out_filename  the output file name.
 * @return                   0 - success.
 *                          <0 - -errno error code.
 */
static int find_zero_memory_pages(const char* out_filename)
{
	page_size = getpagesize();

	int fd_out = creat(out_filename, 0644);
	if (fd_out == -1) return -errno;

	int rc = parse_maps(fd_out);
	close(fd_out);
	return rc;
}

/**
 * Enables/disables tracing.
 *
 * @param[in] value   enable tracing if true, otherwise disable it.
 * @return
 */
static void enable_tracing(bool value)
{
	if (!value && trace_enabled) {
		char filename[PATH_MAX];
		/* copy /proc/self/maps file */
		sp_rtrace_get_out_filename("pagemap-maps", filename, sizeof(filename));
		sp_rtrace_attachment_t file_maps = {
				.name = "maps",
				.path = filename,
		};
		sp_rtrace_copy_file("/proc/self/maps", filename);
		sp_rtrace_write_attachment(&file_maps);

		/* copy /proc/self/pagemap file */
		sp_rtrace_get_out_filename("pagemap-pagemap", filename, sizeof(filename));
		sp_rtrace_attachment_t file_pagemap = {
				.name = "pagemap",
				.path = filename,
		};
		sp_rtrace_copy_file("/proc/self/pagemap", filename);
		sp_rtrace_write_attachment(&file_pagemap);

		/* copy /proc/kpageflags file */
		sp_rtrace_get_out_filename("pagemap-kpageflags", filename, sizeof(filename));
		sp_rtrace_attachment_t file_kpageflags = {
				.name = "kpageflags",
				.path = filename,
		};
		sp_rtrace_copy_file("/proc/kpageflags", filename);
		sp_rtrace_write_attachment(&file_kpageflags);

		/* scan for zero pages */
		sp_rtrace_get_out_filename("pagemap-zeropages", filename, sizeof(filename));
		find_zero_memory_pages(filename);
		sp_rtrace_attachment_t file_zeropages = {
				.name = "zeropages",
				.path = filename,
		};
		sp_rtrace_write_attachment(&file_pagemap);
	}
	trace_enabled = value;
}


static void trace_pagemap_init(void) __attribute__((constructor));
static void trace_pagemap_fini(void) __attribute__((destructor));


static void trace_pagemap_init(void)
{
	sp_rtrace_register_module(module_info.name, module_info.version_major, module_info.version_minor, enable_tracing);
}

static void trace_pagemap_fini(void)
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
