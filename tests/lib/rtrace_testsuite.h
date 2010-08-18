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

#ifndef RTRACE_TESTSUITE_H
#define RTRACE_TESTSUITE_H

/**
 * @file rtrace_testsuite.h
 *
 * Simple testsuite implementation.
 *
 * 1) The testsuite must be initialized at the beginning of file with
 *    RT_INIT()
 * 2) Define test cases with RT_CASE(<test case>) macro.
 * 3) Inside test case perform checks for expected values with
 *    RT_ASSERT, RT_ASSERT_EX macros. If a test case calls a function
 *    witch uses the RT_ASSERT* macros, then the function must return
 *    RT_OK in the case of success and the function call must be checked
 *    with an assert itself: RT_ASSERT(some_function() == RT_OK)
 * 4) Start the testsuite with RT_START(<testsuite name>)
 * 5) Execute test cases with RT_RUN_CASE, RT_RUN_CASE_NO_MEMCHECK macros.
 *
 * The test suite will print \t<test case> ... ok|fail in the standard
 * output, which can be be parsed to determine which test cases passed
 * and which not.
 */

#include <stdbool.h>
#include <malloc.h>

#define RT_SIZEOF(array) (sizeof(array) / sizeof(array[0]))

/**
 * Failure/success definitions
 */
#define RT_OK		0
#define RT_FAIL		-1

/**
 * Initializes test case.
 *
 * This macro must be defined at the beginning of the testsuite.
 */
#define RT_INIT() \
	static bool rt_case_failure_detected = false;

/**
 * Starts the testsuite.
 *
 * This macro must be defined before any testcase has been ran.
 */
#define RT_START(name) \
		printf("Testsuite: %s\n", name);

/**
 * Defines test case.
 *
 * This macro defines the test case function.
 */
#define RT_CASE(name) \
		int rt_case_impl_##name(const char* rt_case_name)

/**
 * Runs the test case.
 *
 * This macro calls the test case function and depending on the return
 * code prints failure of success message.
 * Additionally if memcheck is true it checks if the test case has leaked
 * any memory and returns failure if leaks are detected.
 */
#define RT_RUN_CASE_BASE(name, memcheck) {\
	rt_case_failure_detected = false; \
	struct mallinfo mi1 = mallinfo(); \
	printf("\t" #name " ... "); \
	if (rt_case_impl_##name(#name) == RT_OK) { \
		struct mallinfo mi2 = mallinfo(); \
		if (memcheck && mi1.uordblks != mi2.uordblks) {\
			printf("fail\n\t\tmemory leak detected (%d)\n", mi2.uordblks - mi1.uordblks); \
			return RT_FAIL; \
		} \
		printf("ok\n"); \
	} \
	else {\
		return RT_FAIL; \
	} \
	}

/**
 * Runs the test case with memory leak checks.
 */
#define RT_RUN_CASE(name) \
		RT_RUN_CASE_BASE(name, true)

/**
 * Runs the test case without memory leak checks.
 */
#define RT_RUN_CASE_NO_MEMCHECK(name)  \
		RT_RUN_CASE_BASE(name, false)

/**
 * The generic assert macro.
 *
 * This macro checks if the expression produces the expected value
 * and prints a failure message otherwise.
 */
#define RT_ASSERT_GENERIC(expr, format, ...) \
		if (!(expr)) { \
			if (!rt_case_failure_detected) { \
				rt_case_failure_detected = true; \
				printf("fail\n"); \
			} \
			printf("\t\t%s" format, #expr, __VA_ARGS__); \
			return RT_FAIL; \
		}

/**
 * Simple assert macro
 */
#define RT_ASSERT(expr) \
		RT_ASSERT_GENERIC(expr, "%c", '\n')


/**
 * Assert macro with additional data which is printed in
 * the case of failure.
 */
#define RT_ASSERT_EX(expr, format, ...) \
		RT_ASSERT_GENERIC(expr, ", " format "\n" , __VA_ARGS__)




#endif
