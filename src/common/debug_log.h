/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010 by Nokia Corporation
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

#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H

/**
 * @file debug_log.h
 *
 * Provides debug logging functionality.
 *
 * TODO: Currently logging is used only when needed to debug
 * some things and removed afterwards. Full logging integration
 * (with multiple severity levels) might be nice.
 */

#ifdef DEBUG_INFO
 #define LOG(format, ...) { fprintf(stderr, "["__FILE__":%s] ", __func__); printf(format, ## __VA_ARGS__); putchar('\n');}
#else
 #define LOG(...)
#endif

#endif
