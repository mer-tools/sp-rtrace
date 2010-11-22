#ifndef _FILTER_MANAGER_H_
#define _FILTER_MANAGER_H_

#include "timeline.h"
#include "filter.h"

/**
 * Provides event filter management.
 *
 * The event filters are created during command line argument
 * parsing and added to FilterManager.
 * Later FilterManager is used by Processor to check if the
 * function call events matches the specified filters.
 */
class FilterManager {
private:
	// the filter list
	typedef std::list<filter_ptr_t> filters_t;
	filters_t filters;

public:

	/**
	 * Gets the filter manager instance.
	 * @return
	 */
	static FilterManager* getInstance() {
		static FilterManager instance;
		return &instance;
	}

	/**
	 * Adds filter to FilterManger.
	 *
	 * The filter is stored in the filter list and
	 * destroyed when the FilterManager is destroyed.
	 *
	 * @param[in] filter   the filter to add.
	 */
	void addFilter(Filter* filter) {
		filters.push_back(filter_ptr_t(filter));
	}

	/**
	 * Checks if the event passes filter validation.
	 *
	 * @param[in] event  the event to validate.
	 * @return           true if the event passes filter validation,
	 *                   false otherwise.
	 */
	bool validate(const Event* event);

};



#endif

