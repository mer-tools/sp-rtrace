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

#ifndef _HISTOGRAM_GENERATOR_H_
#define _HISTOGRAM_GENERATOR_H_

#include "timeline.h"

#include "report_generator.h"
#include "report_data.h"
#include "event.h"

/**
 * The base class for histogram generators.
 *
 * The histogram generators generates statistics about allocation
 * count/total size per resource size.
 * This class contains the common implementation for count and
 * size histogram generation.
 */
class HistogramGenerator: public ReportGenerator {
protected:

	/**
	 * Tracks number of total and freed allocations.
	 */
	struct Alloc {
		// total number of allocated resources
		unsigned int total;
		// total number of freed resources
		unsigned int freed;

		/**
		 * Creates a new class instance.
		 */
		Alloc()
			: total(0), freed(0) {
		}
	};

	/**
	 * Allocation statistics.
	 */
	class Stats {
	public:
		// the number of allocations
		unsigned int count;
		// the total allocation size
		unsigned int total;

		// a list of all sizes of the allocated resources
		typedef std::vector<unsigned int> allocs_t;
		allocs_t allocs;

		/**
		 * Creates a new class instance.
		 */
		Stats()
			: count(0), total(0) {
		}

		/**
		 * Calculates the median allocation size.
		 *
		 * @return  the median allocation size.
		 */
		unsigned int getMedian();
	};

	/**
	 * The resource related data container
	 */
	class ResourceData: public ResourceDataBase<int> {
	public:
		// allocation size->(total count, freed count) mapping.
		// Used to store statistisc for all allocation sizes of the resource type.
		typedef std::map<unsigned int, Alloc> data_t;
		data_t allocs;
	};

	// the used resource types
	ReportData<ResourceData> resources;

	/**
	 * Writes histogram record into data file.
	 *
	 * Override this method to provide specific (count|size) histogram implementation.
	 *
	 * @param[in] file    the data file.
	 * @param[in] alloc   the allocation data.
	 * @param[in] size    the allocation size.
	 */
	virtual void writeAlloc(Plotter::DataFile* file, Alloc& alloc, unsigned int size) = 0;


public:
	// Y axis range
	unsigned int yrange_max;

	// Y axis label
	std::string ylabel;
	// the associated resource type.
	const Resource* resource_type;
	// the histogram mode (size|count)
	std::string mode;
	// the report title
	std::string title;

	// the vector type used for size data ordering
	typedef std::vector<unsigned int> keys_t;

	/**
	 * Creates a new class instance.
	 *
	 * @param[in] mode    the histogram mode ("size" | "count").
	 * @param[in] title   the histogram title.
	 * @param[in] ylabel  the Y axis name.
	 */
	HistogramGenerator(const std::string& mode, const std::string& title, const std::string& ylabel)
		: ReportGenerator("histogram-" + mode), yrange_max(0),
		  ylabel(ylabel), resource_type(NULL), mode(mode), title(title) {
	}

	/**
	 * @copydoc ReportGenerator::reportAlloc
	 */
	int reportAlloc(const Resource* resource, event_ptr_t& event);

	/**
	 * @copydoc ReportGenerator::reportAllocInContext
	 */
	int reportAllocInContext(const Resource* resource, const Context* context, event_ptr_t& event) {
		// context data are ignored for histograms
		return OK;
	}

	/**
	 * @copydoc ReportGenerator::reportFree
	 */
	int reportFree(const Resource* resource, event_ptr_t& event, event_ptr_t& alloc_event);

	/**
	 * @copydoc ReportGenerator::reportFreeInContext
	 */
	int reportFreeInContext(const Resource* resource, const Context* context, event_ptr_t& event, event_ptr_t& alloc_event) {
		return OK;
	}

	/**
	 * @copydoc ReportGenerator::reportUnfreedAlloc
	 */
	int reportUnfreedAlloc(const Resource* resource, event_ptr_t& event) {
		return OK;
	}

	/**
	 * @copydoc ReportGenerator::finalize
	 */
	void finalize();
};


/**
 * The size histogram generator.
 */
class SizeHistogramGenerator: public HistogramGenerator {
protected:
	/**
	 * @copydoc HistogramGenerator::writeAlloc
	 */
	void writeAlloc(Plotter::DataFile* file, Alloc& alloc, unsigned int size);

public:

	/**
	 * Creates a new class instance.
	 */
	SizeHistogramGenerator()
		: HistogramGenerator("size", "Total allocation size per resource size", "Total allocation size") {
	}
};

class CountHistogramGenerator: public HistogramGenerator {
protected:
	/**
	 * @copydoc HistogramGenerator::writeAlloc
	 */
	void writeAlloc(Plotter::DataFile* file, Alloc& alloc, unsigned int size);

public:

	/**
	 * Creates a new class instance.
	 */
	CountHistogramGenerator()
		: HistogramGenerator("count", "Allocation count per resource size", "Allocation count") {
	}

};


#endif

