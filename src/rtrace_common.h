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

#include "config.h"

/**
 * @file rtrace_common.h
 *
 * Common definitions.
 */

/* the tracing module installation directory and path*/
#define SP_RTRACE_LIB_DIR          "sp-rtrace"
#define SP_RTRACE_LIB_PATH         INSTALL_DIR "/lib/" SP_RTRACE_LIB_DIR "/"

/* the main tracing module */
#define SP_RTRACE_MAIN_MODULE      "libsp-rtrace-main.so"

/* the audit tracing module */
#define SP_RTRACE_AUDIT_MODULE      "libsp-rtrace-audit.so"

/* pre-processor  */
#define SP_RTRACE_PREPROC          "sp-rtrace"

/* post-processor */
#define SP_RTRACE_POSTPROC         "sp-rtrace-postproc"

/* resolver */
#define SP_RTRACE_RESOLVER         "sp-rtrace-resolve"

/* event pre-processor named pipe path template */
#define SP_RTRACE_PIPE_PATTERN      "/tmp/rtrace-"

/* the text file pattern, %d-%d - <pid>-<index> */
#define SP_RTRACE_TEXT_FILE_PATTERN   "%d-%d.rtrace.txt"

/* the binary file pattern,  %d-%d - <pid>-<index> */
#define SP_RTRACE_BINARY_FILE_PATTERN   "%d-%d.rtrace"

#endif /* RTRACE_COMMON_H */
