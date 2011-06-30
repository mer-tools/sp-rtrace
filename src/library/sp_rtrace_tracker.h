/**
 * @file sp_rtrace_tracker.h
 *
 * Tracker API provides functionality to parse/query tracked
 * library/function setup configuration.
 *
 * The configuration string has the following format:
 * \<item\>[;\<item\>[;...]]
 * Where \<item\> is either contains function name to track
 * or configuration file name in format @<filename>.
 * The @ symbols is used to distinguish symbol name form
 * configuration file name.
 *
 * If an item ends with '*' then partial match is performed.
 */
#ifndef _SP_RTRACE_TRACKER_H_
#define _SP_RTRACE_TRACKER_H_

#define _GNU_SOURCE

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * The function/library tracking data.
 */
typedef struct sp_rtrace_tracker_t {
	/* the function names to track */
	void* symbols;
} sp_rtrace_tracker_t;


/**
 * Initializes tracker.
 *
 * @param[in] tracker   the tracker.
 * @param[in] setup     the configuration string.
 * @return              0 - success.
 */
int sp_rtrace_tracker_init(sp_rtrace_tracker_t* tracker, const char* setup);

/**
 * Frees resources allocated by the tracker.
 *
 * @param[in] tracker  the tracker.
 */
void sp_rtrace_tracker_free(sp_rtrace_tracker_t* tracker);

/**
 * Checks if the specified function is being tracked.
 *
 * Note that this function allocates memory for the found symbol
 * storage and the caller is responsible for releasing it.
 * @param[in] tracker  the tracker.
 * @param[in] name     the function name.
 * @return             The tracked symbol (demangled) or NULL.
 *
 */
char* sp_rtrace_tracker_query_symbol(sp_rtrace_tracker_t* tracker, const char* name);

#ifdef  __cplusplus
}
#endif


#endif

