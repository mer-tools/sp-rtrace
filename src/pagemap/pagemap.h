#ifndef _PAGEMAP_H_
#define _PAGEMAP_H_

#include <stdint.h>

/**
 * Additional page information flags.
 */
enum pageinfo_t {
	PAGE_SWAP = 1 << 0,  //!< PAGE_SWAP
	PAGE_MEMORY = 1 << 1,//!< PAGE_MEMORY
	PAGE_ZERO = 1 << 2,  //!< PAGE_ZERO
};

/**
 * Memory page data.
 */
typedef struct {
	/* page information (pageinfo_t enum) */
	unsigned int info;
	/* page flags (from /proc/kpageflags */
	uint64_t kflags;
} pageflags_data_t;

/**
 * Memory page data header.
 *
 * This structure contains information about the memory area
 * described by the following page data.
 */
typedef struct {
	/* the memory are start address */
	unsigned long from;
	/* the memory area end address */
	unsigned long to;
	/* size of the following page data */
	unsigned int size;
} pageflags_header_t;

#endif
