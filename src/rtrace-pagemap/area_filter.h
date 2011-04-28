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
#include "pagemap.h"

/**
 * @file area_filter.h
 *
 * Provides memory area filters.
 */


/**
 * Basic memory area filter.
 *
 * This is the default filter (when no area filtering options are specified).
 * It validates any area and is used as base class for area filters.
 */
class AreaFilter {
public:

	/**
	 * Performs area validation.
	 *
	 * @param[in] area  the area to validate.
	 * @return          true if the area passes the filter settings.
	 */
	virtual bool validate(MemoryArea* area) {
		return true;
	}
};

/**
 * The path based area filter implementation.
 *
 * This filter validates areas with modules matching the specified path.
 */
class PathFilter : public AreaFilter {
private:
	std::string path;
public:
	/**
	 * Creates a new class instance.
	 *
	 * @param[in] path  the path validation string.
	 */
	PathFilter(const std::string& path) : path(path) {
	}

	/**
	 * Performs area validation.
	 *
	 * This function returns true if the area path contains the path substring
	 * specified in filter constructor.
	 * @param[in] area  the area to validate.
	 * @return          true if the area passes the filter settings.
	 */
	bool validate(MemoryArea* area) {
		return area->path.find(path) != std::string::npos;
	}
};


/**
 * The address based area filter implementation
 *
 * This filter validates area to which the specified address belongs.
 */
class AddressFilter : public AreaFilter {
private:
	unsigned long address;
public:

	/**
	 * Creates a new class instance.
	 */
	AddressFilter(unsigned long address) : address(address) {
	}

	/**
	 * Performs area validation.
	 *
	 * This function returns true if the area contains the address
	 * specified in filter constructor.
	 * @param[in] area  the area to validate.
	 * @return          true if the area passes the filter settings.
	 */
	bool validate(MemoryArea* area) {
		return area->from <= address && address < area->to;
	}
};
