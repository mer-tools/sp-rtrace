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

#ifndef PARSE_TEXT_H
#define PARSE_TEXT_H

#include "common/rtrace_data.h"

/**
 * @file parse_text.h
 *
 * Text parser implementation.
 *
 * The text parser reads text format data from the
 * input stream (defined by --input-file option), parses
 * it and stores the resulting data into rd_t data structure.
 * See rtrace_data.h for rd_t structure details and PROTO_TEXT
 * file for text format specifications.
 */

/**
 * Processes text format data.
 *
 * @param[out] rd   the resource trace data container.
 * @param[in] fp    the input stream.
 * @return
 */
void process_text_data(rd_t* rd, FILE* fp);

#endif
