/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010,2011 by Nokia Corporation
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
#ifndef _LIFETIME_GENERATOR_
#define _LIFETIME_GENERATOR_

#include "timeline.h"

#include "report_generator.h"
#include "report_data.h"
#include "event.h"

/**
 * The LifetimeGenerator class generates resource life time
 * report. 
 * 
 * This report contains lines displaying resource allocation 
 * and deallocation(free) times.
 * 
 * If less than DETAILS_LIMIT resources are found in the log, then
 * every resource lifeline is marked with a different color. 
 * Otherwise lifelines are marked by allocation contexts.
 */
class LifetimeGenerator: public ReportGenerator {
private:

	// the maximum number of resources to use resource resolution
	enum {DETAILS_LIMIT = 20};

	/**
	 * The allocation statistics for minimal and maximal
	 * resoure sizes.
	 */
	class Stats {
	public:
		/**
		 * The single statistics data.
		 */
		class Data {
		public:
			// allocation size
			unsigned int size;
			// number of allocation of this size
			unsigned int count;
			// time of the first allocation of this size
			timestamp_t timestamp;

			/**
			 * Creates a new class instance.
			 */
			Data(unsigned int size = 0)
				: size(size), count(0), timestamp(0) {
			}
		};

		// the minimal size statistics 
		Data min;
		// the maximal size statistics
		Data max;

		/**
		 * Creates a new class instance.
		 */
		Stats()
			: min(INT_MAX) {
		}
	};

	/**
	 * The resource related data storage.
	 */
	class ResourceData: public ResourceDataBase<int> {
	public:
		// event resolution data files
		typedef std::list<Plotter::DataFile*> events_t;
		events_t event_files;

		// context resolution data files
		typedef std::map<context_t, Plotter::DataFile*> contexts_t;
		contexts_t context_files;

		// allocation sizes for median value calculation
		typedef std::vector<int> allocs_t;
		allocs_t allocs;

		// statistics data
		Stats stats;
		
		// the total allocation size
		unsigned int total;

		/**
		 * Creates a new class instance.
		 */
		ResourceData() : total(0) {
		}
	};

	/**
	 * Registers resource life line. 
	 * 
	 * @param[in] rd             the resource type.
	 * @param[in] event          the resource allocation event.
	 * @param[in] end_timestamp  the resource deallocation timestamp (can be also maximum X
	 *                           axis value, if the resource was not freed).
	 */
	void registerLifeline(ResourceData* rd, event_ptr_t& event, timestamp_t end_timestamp);

	// the used resource types
	ReportData<ResourceData> resources;

public:
	// X Axis range
	unsigned int xrange_min;
	unsigned int xrange_max;
	// Y axis range
	unsigned int yrange_min;
	unsigned int yrange_max;

	// the number of registered lifelines
	unsigned int total_lifelines;

	/**
	 * Creates a new class instance.
	 */
	LifetimeGenerator()
		: ReportGenerator("lifetime"), xrange_min(-1), xrange_max(0), yrange_min(1), yrange_max(0), total_lifelines(0) {
	}

	/**
	 * @copydoc ReportGenerator::reportAlloc 
	 */
	int reportAlloc(const Resource* resource, event_ptr_t& event);
	
	
	/**
	 * @copydoc ReportGenerator::reportAllocInContext 
	 */
	int reportAllocInContext(const Resource* resource, const Context* context, event_ptr_t& event) {
		return OK;
	}

	/**
	 * @copydoc ReportGenerator::reportFree 
	 */
	int reportFree( const Resource* resource, event_ptr_t& event, event_ptr_t& alloc_event) {
		registerLifeline(resources.getData(resource), alloc_event, event->timestamp);
		return OK;
	}

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
		registerLifeline(resources.getData(resource), event, xrange_max);
		return OK;
	}

	/**
	 * @copydoc ReportGenerator::finalize
	 */
	void finalize();
	
};






#endif

