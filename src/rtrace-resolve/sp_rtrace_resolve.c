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
 * @file sp_rtrace_resolve.c
 *
 * Resource trace resolver (sp-rtrace-resolve) implementation.
 *
 * The name resolving logic has been mostly taken from the
 * functracer-postproc name resolving routines.
 *
 * By default resolver caches symbols from all libraries mapped
 * by the target process. While this is the fastest mode, it
 * also takes a lot of memory, which can be problematic in
 * low memory environment.
 * In this case multi-pass resolving can be used (--multi-pass
 * option). In this mode the input stream is split into main
 * index file and separate address files for each module. Then
 * the address files are resolved and the output is assembled
 * from the index and resolved address files.
 * The last mode (--single-cache option) is similar to the first
 * mode, but instead of keeping cache for every opened module,
 * a single cache is used. This is the slowest (by a lot) mode.
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <bfd.h>
#include <libiberty.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>

#include "common/utils.h"
#include "common/rtrace_data.h"
#include "common/sp_rtrace_defs.h"
#include "common/header.h"
#include "library/sp_rtrace_formatter.h"
#include "namecache.h"
#include "resolver.h"
#include "sp_rtrace_resolve.h"


resolve_options_t resolve_options = {
	.input_file = NULL,
	.output_file = NULL,
	.mode = MODE_DEFAULT,
	.full_path = false,
	.keep_resolved = false,
};


static volatile sig_atomic_t resolver_abort = 0;

/**
 * Free options resources allocated during command line parsing.
 *
 * @return
 */
static void free_options()
{
	if (resolve_options.input_file) free(resolve_options.input_file);
	if (resolve_options.output_file) free(resolve_options.output_file);
}


static void display_usage()
{
	printf( "sp-rtrace-resolve name resolver is used for stack trace function\n"
			"name resolving. It accepts text output produced by post-processor\n"
			"or itself and generates the same format text data.\n"
			"Usage: sp-rtrace-resolve [<options>]\n"
			"where <options> are:\n"
			"  -i <path>    - the input file path. Standard input used by default.\n"
			"  -o <path>    - the output file path. Standard output is used by default.\n"
			"  -m <mode>    - The operation mode, where <mode> can be either multi-pass or\n"
			"                 single-cache.\n"
			"  -t <method>  - The resolving method, where <method> can be either elf or\n"
			"                 bfd.\n"
			"  -p           - keep the path of the source file (by default the path\n"
			"                 is stripped leaving only the file name.\n"
			"  -k           - keep resolved names (by default the resolved names\n"
			"                 from input stream are ignored and the addresses are\n"
			"                 always resolved again).\n"
			"  -h           - this help page.\n"
	);
}

/**
 * Parses and indexes backtrace record.
 *
 * This function reads backtrace record from line, writes
 * it into address file of the source module and returns
 * the module memory mapping record index.
 * @param[in] line   the line to parse.
 * @param[in] rs     the resolver cache.
 * @return           index of address source module.
 */
static int parse_backtrace_index(const char* line, rs_cache_t* rs)
{
	pointer_t addr;
	char delim;
	if (sscanf(line, "%c0x%lx", &delim, &addr) == 2 && delim == '\t') {
		rs_mmap_t* mmap = rs_mmap_find_module(&rs->mmaps, addr);
		if (mmap == NULL) {
			return -EINVAL;
		}
		fputs(line, mmap->fin);
		return mmap->id;
	}
	return -EINVAL;
}

/**
 * Reads memory mapping record from the line.
 *
 * This function attempts to read memory mapping record from the
 * specified line and adds the mapping to the mmaps array if
 * successful.
 * @param[in] line   the line to parse.
 * @param[in] rs     the resolver cache.
 * @return
 */
static const char* parse_mmap_record(const char* line, rs_cache_t* rs)
{
	char module[PATH_MAX];
	pointer_t from, to;
	if (sscanf(line, ": %s => 0x%lx-0x%lx", module, &from, &to) == 3) {
		rs_mmap_t* mmap = rs_mmap_add_module(rs, module, from, to,
				!(resolve_options.mode & MODE_FULL_CACHE));
		if (mmap && (resolve_options.mode & MODE_MULTI_PASS)) {
			/* create module address files for multi-pass resolving */
			char path[PATH_MAX];
			char* ptr = strrchr(mmap->module, '/');
			if (ptr) ++ptr;
			else ptr = mmap->module;

			sprintf(path, "rtrace.in.%s", ptr);
			mmap->fin = fopen(path, "w+");
			if (mmap->fin == NULL) {
				fprintf(stderr, "ERROR: failed to create module address file %s\n", path);
				exit(-1);
			}
			sprintf(path, "rtrace.out.%s", ptr);
			mmap->fout = fopen(path, "w+");
			if (mmap->fout == NULL) {
				fprintf(stderr, "ERROR: failed to create module address file %s\n", path);
				exit(-1);
			}
		}
		return line;
	}
	return NULL;
}

/**
 * Parses index record from the line.
 *
 * This function checks if the line contains index record and
 * replaces it with the resolved backtrace record from the
 * module resolved address file.
 * @param[in] line  the line to parse.
 * @param[in] rs    the resolver cache.
 */
static void parse_index_record(char* line, rs_cache_t* rs)
{
	int index;
	if (sscanf(line, "^%d", &index) == 1) {
		rs_mmap_t* mmap = rs->mmaps_index[index];
		if (fgets(line, PATH_MAX, mmap->fout) == 0) {
			fputs(line, stderr);
			fprintf(stderr, "ERROR: multi-pass resolving failed, unexpected eof "
							" of %s resolved data\n", mmap->module);
			exit(-1);
		}
	}
}

/**
 * Parses and resolves backtrace record.
 *
 * This function attempts to scan @p line for backtrace record.
 * If the record is found, it's replaced with resolved backtrace record.
 * @param[in,out] pout   the line containing backtrace record.
 * @return
 */
static const char* parse_backtrace_record(char* line, rs_cache_t* rs)
{
	pointer_t addr;
	char resolved_name[PATH_MAX] = "", delim;
	int n = sscanf(line, "%c0x%lx (%[^\n]", &delim, &addr, resolved_name);
	if (n < 2 || delim != '\t') return NULL;
	if (n == 3) resolved_name[strlen(resolved_name) - 1] = '\0';
	return rs_resolve_address(rs, addr, resolved_name);
}

/**
 * Resolves address file of the specified module.
 *
 * @param[in] mmap  the module memory mapping record.
 * @param]in] rs    the resolver cache.
 * @return
 */
static void mmap_resolve_address_file(rs_mmap_t* mmap, rs_cache_t* rs)
{
	char line[PATH_MAX];

	fseek(mmap->fin, 0, SEEK_SET);
	while (fgets(line, PATH_MAX, mmap->fin)) {
		if (resolver_abort) return;
		pointer_t address;
		char resolved_name[PATH_MAX] = "";
		int n = sscanf(line, "\t0x%lx (%[^\n]",  &address, resolved_name);
		if (n < 1) {
				fprintf(stderr, "WARNING: unexpected string in module address file: %s", line);
		}
		else {
			if (n == 2) resolved_name[strlen(resolved_name) - 1] = '\0';
			if (fputs(rs_resolve_address(rs, address, resolved_name), mmap->fout) == EOF) {
				fprintf(stderr, "ERROR: while writing resolved data to index file (%s)\n", strerror(errno));
				exit (-1);
			}
		}
	}
	fclose(mmap->fin);
	fseek(mmap->fout, 0, SEEK_SET);
}

/**
 * Close resolved address file and removes address and
 * resolved address files of the specified module.
 * @param[in] mmap  the module.
 */
static void mmap_cleanup_tmp_files(rs_mmap_t* mmap)
{
	fclose(mmap->fout);

	char path[PATH_MAX];
	char* ptr = strrchr(mmap->module, '/');
	if (ptr) ++ptr;
	else ptr = mmap->module;

	sprintf(path, "rtrace.in.%s", ptr);
	remove(path);
	sprintf(path, "rtrace.out.%s", ptr);
	remove(path);
}
/**
 * Resolves backtrace addresses in @p fpin file and writes output
 * to @p fpout file.
 * @param[in] fpin    the input file.
 * @param[in] fpout   the output file.
 * @return
 */
static void do_resolve(FILE *fpin, FILE *fpout)
{
	rs_cache_t rs;
	rs_cache_init(&rs);
	char line[4096];

    if (resolve_options.mode & MODE_MULTI_PASS) {
		FILE* findex = fopen("rtrace.index", "w+");
		if (!findex) {
			fprintf(stderr, "ERROR: failed to create multi-pass index file\n");
			exit(-1);
		}
		/* 1. step - index the input stream */
		while (fgets(line, sizeof(line), fpin)) {
			if (resolver_abort) return;
			const char* pout = NULL;
			pout = parse_mmap_record(line, &rs);
			if (!pout) {
				int index = parse_backtrace_index(line, &rs);
				/* backtrace record with memory mapping, index it */
				if (index >= 0) {
					if (fprintf(findex, "^%d\n", index) == 0) {
						fprintf(stderr, "ERROR: while writing index data to file (%s)\n", strerror(errno));
						exit (-1);
					}
					continue;
				}
				pout = line;
			}
			fputs(pout, findex);
		}

		/* 2. step - resolve the indexed files */
		sarray_foreach2(&rs.mmaps, (op_binary_t)mmap_resolve_address_file, (void*)&rs);

		/* 3. step - assemble the output file */
		fseek(findex, 0, SEEK_SET);

		while (fgets(line, sizeof(line), findex)) {
			if (resolver_abort) return;
			parse_index_record(line, &rs);
			if (fputs(line, fpout) == EOF) {
				fprintf(stderr, "ERROR: while writing assembled data to file (%s)\n", strerror(errno));
				exit (-1);
			}
		}

		/* clean up the temporary files */
		sarray_foreach(&rs.mmaps, (op_unary_t)mmap_cleanup_tmp_files);
		fclose(findex);
		remove("rtrace.index");
	}
	else {
		while (fgets(line, sizeof(line), fpin)) {
			if (resolver_abort) return;
			const char* pout = NULL;
			pout = parse_mmap_record(line, &rs);
			if (!pout) pout = parse_backtrace_record(line, &rs);
			if (!pout) pout = line;
			if (fputs(pout, fpout) == EOF) {
				fprintf(stderr, "ERROR: while writing resolved data to file (%s)\n", strerror(errno));
				exit (-1);
			}
		}
	}
	rs_cache_free(&rs);
}

/**
 * Copies header from input to output stream.
 * 
 * This function reads header line (the firt line in source stream), parses it's
 * fields, checks architecture, appends 'resolve' filter tag and writes the resulting
 * header into output stream.
 * @param[in] fpin   the input stream.
 * @param[in] fpout  the output stream.
 */
static void read_header(FILE* fpin, FILE* fpout)
{
	char line[4096];
	if (fgets(line, sizeof(line), fpin) == NULL) {
		fprintf(stderr, "ERROR: while reading input stream (%s)\n", strerror(errno));
		exit (-1);
	}
	sp_rtrace_header_t header;
	header_read(&header, line);
	
	/* check source stream architecture */
	if (header.fields[SP_RTRACE_HEADER_ARCH] && strcmp(header.fields[SP_RTRACE_HEADER_ARCH], BUILD_ARCH)) {
		fprintf(stderr, "ERROR: unsupported architecture: %s (expected %s)\n",
				header.fields[SP_RTRACE_HEADER_ARCH], BUILD_ARCH);
	    exit (-1);
	}
	/* set the resolve filter tag */
	header_set_filter(&header, header_get_filter(&header) | FILTER_MASK_RESOLVE);
	/* write the header into output file */
	sp_rtrace_print_header(fpout, &header);
	header_free(&header);
}

/**
 * Resolves backtrace data.
 *
 * This function prepares input and output streams and calls
 * @p do_resolve function to do the actual name resolving.
 * @return
 */
static void resolve()
{
	FILE *fpin = stdin, *fpout = stdout;
	/* read data from the specified file or standard input if no
	 * input file was specified with --input-file option */
	if (resolve_options.input_file) {
		fpin = fopen(resolve_options.input_file, "r");
		if (fpin == NULL) {
			fprintf(stderr, "ERROR: Failed to open input file %s (%s)\n",
					resolve_options.input_file, strerror(errno));
			exit (-1);
		}
	}
	if (resolve_options.output_file) {
		fpout = fopen(resolve_options.output_file, "w");
		if (fpout == NULL) {
			fprintf(stderr, "ERROR: Failed to open input file %s (%s)\n",
					resolve_options.input_file, strerror(errno));
			exit (-1);
		}
		printf("INFO: Created text log file %s\n", resolve_options.output_file);
	}
    /* initialize BFD library */
    bfd_init();
    if (namecache_alloc() != 0) {
    	fprintf(stderr, "ERROR: failed to allocate resolved name cache\n");
    	exit (-1);
    }

    read_header(fpin, fpout);
	do_resolve(fpin, fpout);

	namecache_release();

	if (resolve_options.input_file) {
		fclose(fpin);
	}
	if (resolve_options.output_file) {
		fclose(fpout);
	}
}

/**
 * Block the sigint from aborting the program execution.
 *
 * @param sig
 */
static void sigint_handler(int sig __attribute((unused)))
{
	resolver_abort = 1;
}

int main(int argc, char* argv[])
{

	/* install interrupt handler */
	struct sigaction sa = {.sa_flags = 0, .sa_handler = sigint_handler};
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		fprintf(stderr, "ERROR: Failed to install SIGINT handler\n");
		return -1;
	}
	/* command line options */
	struct option long_options[] = {
			 {"input-file", 1, 0, 'i'},
			 {"output-file", 1, 0, 'o'},
			 {"mode", 1, 0, 'm'},
			 {"method", 1, 0, 't'},
			 {"help", 0, 0, 'h'},
			 {"full-path", 0, 0, 'p'},
			 {"keep-resolved", 0, 0, 'k'},
			 {0, 0, 0, 0},
	};
	/* parse command line options */
	int opt;
	opterr = 0;

	while ( (opt = getopt_long(argc, argv, "i:o:hm:pkt:", long_options, NULL)) != -1) {
		switch(opt) {
		case 'h':
			display_usage();
			exit (0);

		case 'i':
			if (resolve_options.input_file) {
				fprintf(stderr, "WARNING: Overriding previously given option: -i %s\n", resolve_options.input_file);
			}
			resolve_options.input_file = strdup_a(optarg);
			break;

		case 'o':
			if (resolve_options.output_file) {
				fprintf(stderr, "WARNING: Overriding previously given option: -o %s\n", resolve_options.output_file);
			}
			resolve_options.output_file = strdup_a(optarg);
			break;

		case 'm':
			if ((resolve_options.mode & MODE_OPERATION_MASK) != MODE_FULL_CACHE) {
				fprintf(stderr, "WARNING: Overriding previously given operation mode option (-m <mode>)\n");
			}
			if (!strcmp(optarg, "multi-pass")) resolve_options.mode = (resolve_options.mode & (~MODE_OPERATION_MASK)) | MODE_MULTI_PASS;
			else if (!strcmp(optarg, "single-cache")) resolve_options.mode = (resolve_options.mode & (~MODE_OPERATION_MASK)) | MODE_SINGLE_CACHE;
			else {
				fprintf(stderr, "ERROR: Unknown operation mode: %s\n", optarg);
				exit(-1);
			}
			break;

		case 't':
			if ((resolve_options.mode & MODE_METHOD_MASK) != (MODE_BFD | MODE_ELF)) {
				fprintf(stderr, "WARNING: Overriding previously given resolving method option (-t <method>)\n");
			}

			if (!strcmp(optarg, "elf")) resolve_options.mode = (resolve_options.mode & (~MODE_METHOD_MASK)) | MODE_ELF;
			else if (!strcmp(optarg, "bfd")) resolve_options.mode = (resolve_options.mode & (~MODE_METHOD_MASK)) | MODE_BFD;
			else {
				fprintf(stderr, "ERROR: Unknown resolving method: %s\n", optarg);
				exit(-1);
			}
			break;

		case 'p':
			resolve_options.full_path = true;
			break;

		case 'k':
			resolve_options.keep_resolved = true;
			break;

		case '?':
			fprintf(stderr, "ERROR: Unknown sp-resolve option: %c\n", optopt);
			display_usage();
			exit (-1);
		}
	}
	if (optind < argc) {
		fprintf(stderr, "ERROR: Unknown sp-rtrace-resolve argument: %s\n", argv[optind]);
		display_usage();
		exit(-1);
	}

	resolve();

	free_options();

	return 0;
}
