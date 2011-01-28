/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2011 by Nokia Corporation
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

