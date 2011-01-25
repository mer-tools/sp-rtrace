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
#include <time.h>
#include <stdint.h>

#include "modules/sp_rtrace_main.h"
#include "modules/sp_rtrace_module.h"
#include "common/sp_rtrace_proto.h"
#include "library/sp_rtrace_defs.h"
#include "common/debug_log.h"
#include "common/utils.h"

#include "pagemap.h"


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


static unsigned long page_size;

/* the parser memory map registration callback */
typedef int (*parser_callback_t)(unsigned long from, unsigned long to, const char* module, const char* access, void* data);

/**
 * The parser data structure.
 */
typedef struct {
	/* the user data */
	void* data;
	/* the registration callback function called for each maps record */
	parser_callback_t process;
} parser_data_t;


/**
 * The data structure used as user data for pagemap file cutting.
 */
typedef struct {
	int fd_in;
	int fd_out;
} pmcut_data_t;



/**
 * The data structure used as user data for kpageflags file cutting.
 */
typedef struct {
	int fd_pf;
	int fd_pm;
	int fd_out;
} pfcut_data_t;

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
 * @param[in] from    the memory area start address.
 * @param[in] to      the memory area end address.
 * @param[in] module  the module(file) mapped to the specified address range.
 * @param[in] rights  the memory are access rights (in the same format as in maps file).
 * @param[in] fd_out  the output file descriptor.
 *
 * @return             0 - success.
 *                    <0 - -errno error code.
 */
static int scan_address_range(unsigned long from, unsigned long to, const char* module, const char* rights, int fd_out)
{
	if (rights[0] != 'r' || rights[1] != 'w' || rights[3] != 'p') return EINVAL;
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
 * Copies pagemap data of the specified memory range into output file.
 *
 * @param[in] from    the memory area start address.
 * @param[in] to      the memory area end address.
 * @param[in] module  the module(file) mapped to the specified address range.
 * @param[in] rights  the memory are access rights (in the same format as in maps file).
 * @param[in] fd_out  the output file descriptor.
 *
 * @return             0 - success.
 *                    <0 - -errno error code.
 */
static int cut_pagemap_range(unsigned long from, unsigned long to, const char* module, const char* rights, pmcut_data_t* data)
{
	if (rights[3] != 'p') return EINVAL;

	unsigned long index = from / page_size;
	unsigned long size = (to - from) / page_size * 8;

	lseek(data->fd_in, index * 8, SEEK_SET);
	char buffer[0x8000];
	unsigned long len = sizeof(buffer) > size ? size : sizeof(buffer);

	while (size) {
		unsigned int n = read(data->fd_in, buffer, len);
		if (n != len) return -EINVAL;
		write(data->fd_out, buffer, len);
		size -= len;
		if (size < len) len = size;
	}
	return 0;
}



/*
 * pagemap kernel ABI bits
 */
#define PM_ENTRY_BYTES      sizeof(uint64_t)
#define PM_STATUS_BITS      3
#define PM_STATUS_OFFSET    (64 - PM_STATUS_BITS)
#define PM_STATUS_MASK      (((1LL << PM_STATUS_BITS) - 1) << PM_STATUS_OFFSET)
#define PM_STATUS(nr)       (((nr) << PM_STATUS_OFFSET) & PM_STATUS_MASK)
#define PM_PSHIFT_BITS      6
#define PM_PSHIFT_OFFSET    (PM_STATUS_OFFSET - PM_PSHIFT_BITS)
#define PM_PSHIFT_MASK      (((1LL << PM_PSHIFT_BITS) - 1) << PM_PSHIFT_OFFSET)
#define PM_PSHIFT(x)        (((u64) (x) << PM_PSHIFT_OFFSET) & PM_PSHIFT_MASK)
#define PM_PFRAME_MASK      ((1LL << PM_PSHIFT_OFFSET) - 1)
#define PM_PFRAME(x)        ((x) & PM_PFRAME_MASK)

#define PM_PRESENT          PM_STATUS(4LL)
#define PM_SWAP             PM_STATUS(2LL)

static int cut_kpageflags_range(unsigned long from, unsigned long to, const char* module, const char* rights, pfcut_data_t* data)
{
	if (rights[3] != 'p') return EINVAL;

	unsigned long index = from / page_size * 8;
	unsigned long end = to / page_size * 8;

	lseek(data->fd_pm, index, SEEK_SET);

	while (index < end) {
		uint64_t page_index;
		read(data->fd_pm, &page_index, sizeof(uint64_t));
		uint64_t page_flags = 0;
		if (page_index & PM_PRESENT) {
			lseek(data->fd_pf, PM_PFRAME(page_flags) * 8, SEEK_SET);
			read(data->fd_pf, &page_flags, sizeof(uint64_t));
		}
		write(data->fd_out, &page_flags, sizeof(uint64_t));
		index += 8;
	}

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
static int parse_record(parser_data_t* data, char* buffer, size_t size)
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

	return data->process(from, to, name_s, rights_s, data->data);
}

/**
 * Parses file buffer for lines.
 *
 * The buffer might contain several lines from /proc/pid/maps.
 * This function separates them and call parse_record() function
 * for each line.
 * @param[in] data        the parser data.
 * @param[in] buffer      the buffer to parse.
 * @param[in] total_size  the number of bytes in buffer.
 * @return                 0 - success.
 *                        <0 - -errno error code.
 */
static int parse_buffer(parser_data_t* data, char* buffer, size_t total_size)
{
	buffer[total_size] = '\0';;
	char* end = buffer;
	while (true) {
		char* ptr = _strnchr(end, '\n', total_size - (end - buffer));
		if (!ptr) break;
		*ptr++ = '\0';
		parse_record(data, end, ptr - end);
		end = ptr;
	}
	return end - buffer;
}

/**
 * Parses /proc/self/maps file.
 *
 * @param[in] data   the parser data.
 * @return              0 - success.
 *                     <0 - -errno error code.
 */
static int parse_maps(parser_data_t* data)
{
	int fd = open("/proc/self/maps", O_RDONLY);
	if (fd == -1) return -errno;

	char buffer[4096];
	int n, offset = 0;
	while ( (n = read(fd, buffer + offset, sizeof(buffer) - offset)) ) {
		size_t total_size = n + offset;
		int nparsed = parse_buffer(data, buffer, total_size);
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
	int fd_out = creat(out_filename, 0644);
	if (fd_out == -1) return -errno;

	parser_data_t data = {
			.data = (void*) fd_out,
			.process = (parser_callback_t)scan_address_range,
	};

	int rc = parse_maps(&data);
	close(fd_out);
	return rc;
}

/**
 * Copies mapped memory area data from /proc/pid/pagemaps file.
 *
 * @param[in]  out_filename  the output file name.
 * @return
 */
static int cut_pagemap(const char* out_filename)
{
	int fd_out = creat(out_filename, 0644);
	if (fd_out == -1) return -errno;

	int fd_in = open("/proc/self/pagemap", O_RDONLY);
	if (fd_in == -1) {
		close(fd_out);
		return -errno;
	}

	pmcut_data_t pm_data = {
			.fd_out = fd_out,
			.fd_in = fd_in,
	};

	parser_data_t data = {
			.data = (void*) &pm_data,
			.process = (parser_callback_t)cut_pagemap_range,
	};

	int rc = parse_maps(&data);
	close(fd_out);
	close(fd_in);
	return rc;
}

static int cut_kpageflags(const char* out_filename)
{
	int fd_out = creat(out_filename, 0644);
	if (fd_out == -1) return -errno;

	int fd_pm = open("/proc/self/pagemap", O_RDONLY);
	if (fd_pm == -1) {
		close(fd_out);
		return -errno;
	}

	int fd_pf = open("/proc/kpageflags", O_RDONLY);
	if (fd_pf == -1) {
		close(fd_out);
		close(fd_pm);
		return -errno;
	}
	pfcut_data_t pf_data = {
			.fd_out = fd_out,
			.fd_pf = fd_pf,
			.fd_pm = fd_pm,
	};

	parser_data_t data = {
			.data = (void*) &pf_data,
			.process = (parser_callback_t)cut_kpageflags_range,
	};

	int rc = parse_maps(&data);
	close(fd_out);
	close(fd_pm);
	close(fd_pf);
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
		page_size = getpagesize();

		struct timespec ts1, ts2;
		clock_gettime(CLOCK_MONOTONIC, &ts1);

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
		//cut_pagemap(filename);
		sp_rtrace_write_attachment(&file_pagemap);

		/* copy /proc/kpageflags file */
		sp_rtrace_get_out_filename("pagemap-kpageflags", filename, sizeof(filename));
		sp_rtrace_attachment_t file_kpageflags = {
				.name = "kpageflags",
				.path = filename,
		};
		sp_rtrace_copy_file("/proc/kpageflags", filename);
		//cut_kpageflags(filename);
		sp_rtrace_write_attachment(&file_kpageflags);

		/* scan for zero pages */
		sp_rtrace_get_out_filename("pagemap-zeropages", filename, sizeof(filename));
		find_zero_memory_pages(filename);
		sp_rtrace_attachment_t file_zeropages = {
				.name = "zeropages",
				.path = filename,
		};
		sp_rtrace_write_attachment(&file_zeropages);

		clock_gettime(CLOCK_MONOTONIC, &ts2);

		int diff = ts2.tv_sec - ts1.tv_sec;
		if (ts2.tv_nsec < ts1.tv_nsec) {
			diff--;
			ts2.tv_nsec += 1000000000;
		}
		diff = diff * 1000 + (ts2.tv_nsec - ts1.tv_nsec) / 1000000;


		fprintf(stderr, "%ld\n", diff);
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
