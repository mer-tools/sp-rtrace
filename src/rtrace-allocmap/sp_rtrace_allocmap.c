/*
 * File: allocmap.c, part of sp-rtrace
 *
 * Copyright (C) 2005,2010 by Nokia Corporation
 *
 * Contact: Eero Tamminen <eero.tamminen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 * Decription:
 *   A program to visualize process heap memory fragmentation on
 *   memory page size bases.  On Linux page size is 4K by default.
 *
 * Notes:
 * - When calling map_dirty_address() for non-freed allocations at the
 *   end of a use-case, memory mapped areas (be default single allocs
 *   >=128KB) should be ignored as they are not allocated from the heap
 *   i.e. they don't fragment it.
 * - After calling map_dirty_address() for all allocations, map_output()
 *   will show which pages in the process virtual memory contain
 *   non-freed memory.
 *
 * Compile with: gcc -O2 -Wall -o allocmap allocmap.c
 *
 * Changes
 *
 * 2005-08-30
 * - original version
 * 2005-09-02
 * - added from Tommi support for showing trim line and reading
 *   keep_top + keepcost from the input file
 * - added support for giving the heap bottom
 *   (as heap is at different address on PC and ARM)
 * 2005-10-20
 * - added support for showing use-case lowest and highest allocation
 * - fix: make sure that trim/high/low markers don't overwrite allocation
 *   marks
 * 2005-11-23
 * - align heap_base on page boundary
 * - support outputting map of multiple libleaks reports + showing
 *   from which reports the leaks came
 * 2005-12-13
 * - Fix to page count calculation, output largest block size
 * 2005-12-16
 * - Option for setting page size (to see fragmentation more in detail)
 * 2010-08-02
 * - Updated to support rtrace format and moved to sp-rtrace package.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#define MAX_MEMORY (64*1024*1024)	/* 64MB */
#define PAGE_SIZE_DEFAULT 4096
#define LINE_SIZE 64

/* heap base address */
static unsigned long heap_base;

/* this array could also be a bitfield, currently it takes 16K */
static unsigned char *usage_map;
static int usage_top, usage_trim, usage_high, usage_low;
static int page_size = PAGE_SIZE_DEFAULT;

/* approximate malloc header size, presumed to preclude allocs */
#define MALLOC_HEADER_SIZE 8


/* Currently just clears the map. If it would be dynamically allocated,
 * this would need to calculate the values that are currently defines
 */
static void map_init(int page_size)
{
	usage_map = calloc(MAX_MEMORY/page_size, 1);
	if (!usage_map) {
		fprintf(stderr, "ERROR: unable to allocate space for usage map!\n");
		exit(1);
	}
}

static void map_set_top(unsigned long address)
{
	int block;
	if (!heap_base) {
		fprintf(stderr, "ERROR: heap base has to be set before heap top!\n");
		exit(1);
	}
	block = (address - heap_base) / page_size;
	if (block > 0 && block > usage_top) {
		usage_top = block;
	} else {
		fprintf(stderr, "WARNING: invalid heap top value 0x%08lx\n", address);
	}
}

static int map_next_unused(int block)
{
	/* find first free unused block to make map more readable */
	for (; block < usage_top; block++) {
		if (!usage_map[block]) {
			return block;
		}
		switch (usage_map[block]) {
		case '|':
		case 'T':
			/* don't pass trim or top marks */
			return 0;
		}
	}
	return block;
}

static void map_set_marks(void)
{
	int block;
	assert(usage_top < MAX_MEMORY/page_size);
	/* heap top marker, has to be first */
	usage_map[usage_top] = 'T';
	/* heap trim marker */
	if (usage_trim && (block = map_next_unused(usage_trim))) {
		usage_map[block] = '|';
	}
	/* heap high marker */
	if (usage_high && (block = map_next_unused(usage_high))) {
		usage_map[block] = '>';
	}
	if (usage_low && !usage_map[usage_low]) {
		/* set low mark only if block is unoccupied */
		usage_map[usage_low] = '<';
	}
}

/* returns into which 4KB block the address belongs to and checks
 * that it's below the heap top.  Heap base and top should have been
 * set before this is called.
 */
static int map_get_block(unsigned long address, const char *error)
{
	int block;
	if (!usage_top) {
		fprintf(stderr, "ERROR: heap top has to be set before heap low/trim/high!\n");
		exit(1);
	}
	block = (address - heap_base) / page_size;
	if (block >= 0 && block <= usage_top) {
		return block;
	} else {
		fprintf(stderr, "WARNING: %s 0x%08lx\n", error, address);
		return 0;
	}
}

static void map_set_trim(unsigned long address)
{
	int block = map_get_block(address, "invalid heap trim value");
	usage_trim = MAX(usage_trim, block);
}
static void map_set_high(unsigned long address)
{
	int block = map_get_block(address, "invalid high address");
	usage_high = MAX(usage_high, block);
}
static void map_set_low(unsigned long address)
{
	int block = map_get_block(address, "invalid low address");
	usage_low = MIN(usage_low, block);
}

/* Marks the 4K item in the map corresponding to given
 * address as being used in the given run.  Returns zero
 * if address is too large.
 */
static int map_dirty_address(unsigned long address, size_t size, int run)
{
	int start, end, i;
	address -= heap_base;
	start = (address-MALLOC_HEADER_SIZE) / page_size;
	end = (address+size) / page_size;

	if (start >= 0 && end < (MAX_MEMORY/page_size)) {
		if (run < '0' || run > '9') {
			run = '#';
		}
		for (i = start; i <= end; i++) {
		  usage_map[i] = run;
		}
		if (end > usage_top) {
		  usage_top = end;
		}
		return 1;
	}
	fprintf(stderr, "Got a invalid address 0x%lx or size %lu\n",
		heap_base + address, size);
	return 0;
}

/* output a map of the pages marked as used
 * return how many pages were dirty
 */
static int map_output(void)
{
	const char *dashes = "----------------------------------------------------------------";
	char map[LINE_SIZE+1];
	int line, i, used, map_lines;
	map_lines = (MAX_MEMORY/page_size/LINE_SIZE);

	printf("Process use-case heap memory usage map.\n");
	printf("Single line corresponds to %dK (0x%04x).\n", LINE_SIZE*page_size/1024, LINE_SIZE*page_size);
	printf("Each '#' char corresponds to %dK page (0x%x) having non-freed data.\n", page_size/1024, page_size);
	printf("'T' marks the top of the heap and '|' to what it could be trimmed.\n");
	printf("'<' marks the lowest and '>' marks the highest allocation during the use-case.\n");
	printf("           .%s.\n", dashes);

	used = 0;
	for (line = 0; line < map_lines; line++) {

		if (line*LINE_SIZE > usage_top) {
		  break;
		}
		printf("0x%08lx |", heap_base + line*LINE_SIZE*page_size);
		for (i = 0; i < LINE_SIZE; i++) {
			char mark = usage_map[line*LINE_SIZE+i];
			switch (mark) {
			case 0:
				map[i] = ' ';
				break;
			case 'T':	/* heap top */
			case '|':	/* heap trim */
			case '<':	/* heap low */
			case '>':	/* heap high */
			case '0':	/* libleaks run number */
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '#':	/* unknown run number */
				map[i] = mark;
				used++;
				break;
			default:
				fprintf(stderr, "Internal ERROR: Unknown memory page marker!\n");
				exit(1);
			}
		}
		map[LINE_SIZE] = '\0';
		printf("%s|\n", map);
	}
	printf("           '%s'\n", dashes);
	return used;
}

static void usage(const char *name, const char *option, const char *error)
{
	fprintf(stderr, "\nERROR in option '%s': %s!\n\n", option, error);
	fprintf(stderr, "usage: %s [options] < allocs\n\n", name);
	fprintf(stderr, "options:\n"
		"\t-s\t\tData contains only single run (allocs marked differently)\n"
		"\t-t <heap-top>\tTop of the heap\n"
		"\t-k <keepcost>\tHow much could be trimmed off the heap top\n"
		"\t-h <address>\tHighest allocated address in the use-case\n"
		"\t\t\t(doesn't take into account size of the allocation)\n"
		"\t-l <address>\tLowest allocated address the use-case\n"
		"\t-b <heap-base>\tBottom address of the heap\n"
		"\t-p <page size>\tPage size to use in output in KB (1,2,4...)\n");
	fprintf(stderr,
		"Alloc format is \"hex-address dec-size\", for example:\n"
		"%s -t 0x08443215 -k 131072\n"
		"0x0834aa90 22\n0x0834ab70 15\n\n", name);
	fprintf(stderr,
		"Heap base and top are given as hexadecimal and trim offset (keepcost)\n"
		"as decimal. They can also be given before the allocs in the input file:\n"
		"# base=0x08100000\n# top=0x0854aa90\n# keepcost=131072\n\n"
		"Input file overrides the command line options (it's parsed later)\n"
		"Base should be set before anything else. Program doesn't check this,\n"
		"the results will just be funny...\n");
	exit(1);
}

static unsigned long read_address(const char *option,
				  int argc, const char *argv[], int i)
{
	unsigned long address;
	if (i >= argc) {
		usage(*argv, option, "Value for the option missing");
	}
	if (sscanf(argv[i], "0x%lx", &address) != 1) {
		usage(*argv, option, "Failed reading the hex value");
	}
	return address;
}

int main(int argc, const char *argv[])
{
	unsigned long address, keepcost = 0, heap_top = 0, heap_base_new = 0;
	unsigned long heap_low = 0, heap_high = 0;
	int i, count, args, used, allocs, psize;
	size_t size, largest;
	int run = '0';	/* first run */

	/* parse args */
	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			/* not an option */
			break;
		}
		if (argv[i][2]) {
			/* not a single letter option */
			usage(*argv, argv[i], "Unknown option");
		}
		switch(argv[i][1]) {
		case 's':
			/* there's only one run */
			run = '#';
			break;
		case 'b':
			i++;
			heap_base = read_address("-b", argc, argv, i);
			break;
		case 'l':
			i++;
			heap_low = read_address("-l", argc, argv, i);
			break;
		case 'h':
			i++;
			heap_high = read_address("-h", argc, argv, i);
			break;
		case 't':
			i++;
			heap_top = read_address("-t", argc, argv, i);
			break;
		case 'p':
			i++;
			if (i >= argc) {
				usage(*argv, "-p", "Value for the option missing");
			}
			psize = atoi(argv[i]);
			if (psize < 1 || psize > 16) {
				usage(*argv, "-p", "Invalid page size");
			}
			page_size = psize * 1024;
			break;
		case 'k':
			i++;
			if (i >= argc) {
				usage(*argv, "-k", "Value for the option missing");
			}
			if (sscanf(argv[i], "%lu", &keepcost) != 1) {
				usage(*argv, "-k", "Failed reading the decimal value");
			}
			break;
		default:
			usage(*argv, argv[i], "Unknown option");
			break;
		}
	}
	/* align heap_base on page size */
	heap_base &= ~(page_size-1);
	map_init(page_size);

	/* parse the input */
	count = 0;
	allocs = 0;
	largest = 0;
	for (;;) {
		char buf[256];

		if (!fgets (buf, sizeof(buf), stdin)) {
			break;
		}
		if (buf[0] == '#') {
			if (allocs) {
				/* next libleaks report */
				count += allocs;
				allocs = 0;
				run++;
			}
			if (1 != sscanf(buf, "# base=0x%lx\n", &heap_base_new) &&
			    1 != sscanf(buf, "# top=0x%lx\n", &heap_top) &&
			    1 != sscanf(buf, "# low=0x%lx\n", &heap_low) &&
			    1 != sscanf(buf, "# high=0x%lx\n", &heap_high) &&
			    1 != sscanf(buf, "# keepcost=%lu\n", &keepcost)) {
				usage(*argv, buf, "Unknown value");
			}
			/* align heap_base on page size */
			heap_base_new &= ~(page_size-1);
			if (heap_base) {
				if (heap_base_new < heap_base) {
					fprintf(stderr, "Previous heap base: 0x%lx\n", heap_base);
					usage(*argv, buf, "Heap base cannot change");
				}
			} else {
				heap_base = heap_base_new;
			}
			continue;
		}

		args = sscanf(buf, "0x%lx %lu\n", &address, &size);
		if (args != 2) {
			usage(*argv, buf, "Unknown value");
		}
		if (size > largest) {
			largest = size;
		}
		map_dirty_address(address, size, run);
		allocs++;
	}
	if (allocs) {
		count += allocs;
	}

	if (heap_top) {
		map_set_top(heap_top);
		if (keepcost) {
			map_set_trim(heap_top - keepcost);
		}
	}
	if (heap_high) {
		map_set_high(heap_high);
	}
	if (heap_low) {
		map_set_low(heap_low);
	}
	map_set_marks();

	/* output the used pages */
	used = map_output();
	printf("Parsed %d unfreed allocations (largest being %lu bytes),\n"
	       "residing on %d pages.  If highest allocation is next to\n"
	       "trim limit, it's blocking freeing of memory.\n",
	       count, largest, used);
	return 0;
}

