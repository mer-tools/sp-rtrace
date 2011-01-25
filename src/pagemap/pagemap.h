#ifndef _PAGEMAP_H_
#define _PAGEMAP_H_

/**
 * The memory page data.
 *
 * This structure is used to store data about memory pages containing
 * only zero bytes. It holds starting page address and the number of
 * consequent pages containing only zero bytes.
 */
typedef struct {
	/* starting area */
	unsigned long addr;
	/* number of pages */
	unsigned long npages;
} pagescan_t;



#endif
