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
#ifndef SP_RTRACE_RESOLVE_H_
#define SP_RTRACE_RESOLVE_H_


/**
 * Operation modes
 */
enum {
	MODE_UNDEFINED = 0x0,
	/* operation modes */
	MODE_FULL_CACHE = 1 << 0,
	MODE_MULTI_PASS = 1 << 1,
	MODE_SINGLE_CACHE = 1 << 2,

	MODE_OPERATION_MASK = MODE_FULL_CACHE | MODE_MULTI_PASS | MODE_SINGLE_CACHE,
	/* resolving modes */
	MODE_BFD = 1 << 16,
	MODE_ELF = 1 << 17,

	MODE_METHOD_MASK = MODE_BFD | MODE_ELF,
	/* */
	MODE_DEFAULT = MODE_FULL_CACHE | MODE_BFD,
};

/**
 * post-processor options
 */
typedef struct resolve_options_t {
	char* input_file;
	char* output_file;
	int mode;
	bool full_path;
	bool keep_resolved;
} resolve_options_t;


extern resolve_options_t resolve_options;


#endif /* SP_RTRACE_RESOLVE_H_ */
