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
#ifndef _REPORT_GENERATOR_H_
#define _REPORT_GENERATOR_H_

#include "timeline.h"

#include "event.h"
#include "plotter.h"
#include "options.h"
#include "common/formatter.h"

/**
 * Base class for report generators.
 *
 * To create a custom report generator:
 * 1) override initialize() method if any Options based initialization
 *    must be done. There are no guarantees that all option values has
 *    been parsed when a generator is created. Therefore any initialization
 *    that depends on some Option values must be done here.
 *
 * 2) Override reportX() methods to process event data. Any specified filters
 *    are already applied.
 *    reportAlloc(), reportFree() methods are used to process data that
 *    doesn't depend on allocation contexts. While reportAllocInContext,
 *    reportFreeInContext are called for every context that matches the
 *    allocation context for the specified event.
 *    For example if we have contexts 1, 2, then:
 *    b) for event with context=0 the following report functions will be
 *       called: reportAlloc(), reportAllocInContext(context=0)
 *    b) for event with context=1 the following report functions will be
 *       called: reportAlloc(), reportAllocInContext(context=1)
 *    c) for event with context=3 the following report functions will be
 *       called: reportAlloc(), reportAllocInContext(context=1),
 *       reportAllocInContext(context=2)
 *    reportXXXInContext() is not automatically called for a context matching
 *    all events. If necessary it can be done manually by calling
 *    reportXXXInContext() from reportXXX() method and passing &context_all
 *    as the context parameter.
 *
 * 3) Override reportUnfreedAlloc() method to process unfreed allocation
 *    data if necessary. After all events are processed this method is called
 *    for every unfreed allocation event.
 *
 * 4) Override finalize() method to processes the accumulated data, setup
 *    the plotter and draw the resulting graphs/statistics.
 */
class ReportGenerator {
protected:

	// the report identifier
	std::string id;

	// the plotter
	Plotter plotter;

	// the context to use for all allocation/deallocation data.
	Context context_all;

public:

	/**
	 * Event reporting return code
	 */
	enum {
		OK = 0,      // event was registered normally
		ABORT = -1,  // event registration failed, generator can't generate report
	};

	/**
	 * Creates the ReportGenerator instance.
	 *
	 * The context_all context is initialized to a mask matching
	 * all events.
	 * @param[in] id  the report identifier specified by the subclass.
	 * @return
	 */
	ReportGenerator(const std::string& id)
		: id(id), context_all(0xFFFFFFFF, "all") {
	}

	/**
	 *
	 * @return
	 */
	virtual ~ReportGenerator() {};

	/**
	 * Reports occurrence of an allocation event.
	 *
	 * @param[in] resource  the allocated resource type.
	 * @param[in] event     the allocation event.
	 */
	virtual int reportAlloc(const Resource* resource, event_ptr_t& event) = 0;

	/**
	 * Reports occurrence of an allocation event in the specified context.
	 *
	 * @param[in] resource  the allocated resource type.
	 * @param[in] context   the allocation context.
	 * @param[in] event     the allocation event.
	 */
	virtual int reportAllocInContext(const Resource*, const Context* context, event_ptr_t& event) = 0;

	/**
	 * Reports occurrence of an deallocation event.
	 *
	 * @param[in] resource     the deallocated resource type.
	 * @param[in] event        the deallocation event.
	 * @param[in] alloc_event  the event that allocated the resource thats is being freed now.
	 */
	virtual int reportFree(const Resource* resource, event_ptr_t& event, event_ptr_t& alloc_event) = 0;

	/**
	 * Reports occurrence of an deallocation event in the specified context.
	 *
	 * @param[in] resource     the deallocated resource type.
	 * @param[in] context      the deallocation context.
	 * @param[in] event        the deallocation event.
	 * @param[in] alloc_event  the event that allocated the resource thats is being freed now.
	 */
	virtual int reportFreeInContext(const Resource* resource, const Context* context, event_ptr_t& event, event_ptr_t& alloc_event) = 0;

	/**
	 * Reports occurrences of all unfreed allocations after all events are processed.
	 *
	 * @param[in] resource   the allocated resource type.
	 * @param[in] event      the allocation event.
	 */
	virtual int reportUnfreedAlloc(const Resource* resource, event_ptr_t& event) = 0;

	/**
	 * Processes the accumulated data and generates the final report.
	 */
	virtual void finalize() = 0;

	/**
	 * Performs generator specific initialization.
	 */
	virtual void initialize() {
	}

	/**
	 * Initializes the generator.
	 */
	virtual void init() {
		plotter.initialize(id);
		initialize();
	}

	/**
	 * Performs cleanup of the allocated resources.
	 */
	virtual void cleanup() {
		plotter.cleanup();
	}

	/**
	 * Finalizes the generator and generates the final report.
	 */
	void finish() {
		finalize();

		std::string out_filename = Formatter() << Options::getInstance()->getOutTemplate() << "-" <<
				id << Options::getInstance()->getOutExtension();
		int fd = creat(out_filename.c_str(), 0666);
		if (fd == -1) {
			throw std::runtime_error(Formatter() << "Failed to create output file: " << out_filename);
		}
		plotter.plot(fd);
		close(fd);
	}
};

typedef std::tr1::shared_ptr<ReportGenerator> report_generator_t;


#endif

