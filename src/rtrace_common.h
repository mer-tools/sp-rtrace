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

#ifndef RTRACE_COMMON_H
#define RTRACE_COMMON_H

/**
 * @file rtrace_common.h
 *
 * Common definitions.
 */

/* the tracing module installation directory and path*/
#define SP_RTRACE_LIB_DIR          "sp-rtrace"
#define SP_RTRACE_LIB_PATH         "/usr/lib/" SP_RTRACE_LIB_DIR "/"

/* the main tracing module */
#define SP_RTRACE_MAIN_MODULE      "libsp-rtrace-main.so"

/* pre-processor  */
#define SP_RTRACE_PREPROC          "sp-rtrace"

/* post-processor */
#define SP_RTRACE_POSTPROC         "sp-rtrace-postproc"

/* resolver */
#define SP_RTRACE_RESOLVER         "sp-rtrace-resolve"

/* event pre-processor named pipe path template */
#define SP_RTRACE_PIPE_PATTERN      "/tmp/rtrace-%d"

/* the text file pattern, %d - pid */
#define SP_RTRACE_TEXT_FILE_PATTERN   "%d"
#define SP_RTRACE_TEXT_FILE_EXT       ".rtrace.txt"

/* the binary file pattern, %d - pid */
#define SP_RTRACE_BINARY_FILE_PATTERN   "%d"
#define SP_RTRACE_BINARY_FILE_EXT       ".rtrace"

#ifndef INSTALL_DIR
 #define INSTALL_DIR "/usr"
#endif

#ifndef BUILD_ARCH
 #define BUILD_ARCH "undefined"
#endif


#endif /* RTRACE_COMMON_H */
