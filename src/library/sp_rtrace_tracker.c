#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <search.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "sp_rtrace_tracker.h"

/* C++ name demangking support */
#define DMGL_PARAMS  (1 << 0) /* Include function args */
#define DMGL_ANSI  (1 << 1)   /* Include const, volatile, etc */

extern char* cplus_demangle(const char* symbol, int flags);


static void tracker_parse_item(sp_rtrace_tracker_t* tracker, char* item);

/**
 * Adds a value to the tracked resource list.
 *
 * The item is either library or function name.
 * @param[in] root   the tracked resource.
 * @param[in] value  the value to add.
 * @return           0 - success, -errno - failure.
 */
static int tree_add_value(void** root, const char* value)
{
	char* item = strdup(value);
	if (!item) return -ENOMEM;
	void* ptr = tsearch(item, root, (int (*)(const void *, const void *))strcmp);
	if (ptr == NULL) return -EINVAL;
	if ( *(char**)ptr != item) {
		free(item);
	}
	return 0;
}

/**
 * Loads configuration data from a file.
 *
 * @param[in] tracker    the tracker.
 * @param[in] filename   the name of file to load.
 * @return               0 - success.
 */
static int tracker_load_config(sp_rtrace_tracker_t* tracker, const char* filename)
{
	/* TODO: implementation */
	FILE* fp = fopen(filename, "r");
	if (fp) {
		char buffer[1024];
		while (fgets(buffer, sizeof(buffer), fp)) {
			int len = strlen(buffer) - 1;
			if (buffer[len] == '\n') buffer[len] = '\0';
			tracker_parse_item(tracker, buffer);
		}
		fclose(fp);
	}
	return 0;
}

/**
 * Parses single configuration item.
 *
 * A configuration item is either tracked function specification
 * in format <function>@<library> or configuration file name.
 * @param[in] tracker   the tracker.
 * @param[in] item      the item to parse.
 */
static void tracker_parse_item(sp_rtrace_tracker_t* tracker, char* item)
{
	/* strip leading and trailing spaces */
	while (*item == ' ') item++;
	char* ptr = item + strlen(item);
	while (*--ptr == ' ') *ptr = '\0';

	/* check if the item contains configuration file name */
	if (*item == '@') {
		tracker_load_config(tracker, item + 1);
		return;
	}
	/* add item to the tracked symbol list */
	tree_add_value(&tracker->symbols, item);
}

/*
 * Public API
 */

int sp_rtrace_tracker_init(sp_rtrace_tracker_t* tracker, const char* setup)
{
	tracker->symbols = NULL;

	if (setup) {
		char* str = strdup(setup);
		if (str == NULL) return -ENOMEM;

		char* item = strtok(str, ";");
		while (item) {
			tracker_parse_item(tracker, item);
			item = strtok(NULL, ";");
		}
		free(str);
	}
	return 0;
}

void sp_rtrace_tracker_free(sp_rtrace_tracker_t* tracker)
{
	if (tracker->symbols) tdestroy(tracker->symbols, free);
}


const char* sp_rtrace_tracker_query_symbol(sp_rtrace_tracker_t* tracker, const char* name)
{
	char* demangled_name = (char*)cplus_demangle(name, DMGL_ANSI | DMGL_PARAMS);
	void* ptr = tfind(demangled_name ? demangled_name : name, &tracker->symbols, (int (*)(const void *, const void *))strcmp);
	if (demangled_name) free(demangled_name);
	return ptr ? *(char**)ptr : NULL;
}
