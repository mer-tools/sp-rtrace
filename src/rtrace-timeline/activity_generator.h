#ifndef _ACTIVITY_GENERATOR_H_
#define _ACTIVITY_GENERATOR_H_

#include "timeline.h"

#include "report_generator.h"
#include "report_data.h"
#include "event.h"

/**
 * Activity report generator.
 *
 * 	The activity report contains graphs displaying the resource allocation
 *  rate per time slice. The allocation rate is calculated by going through
 *  events by time based steps and calculating the allocation rate for each
 *  step by dividing the total allocation size with total allocation number
 *  for all allocations during the last time slice period.
 *  Usually the step value is set to half of the specified time slice.
 */
class ActivityGenerator: public ReportGenerator {
private:

	void out(int a, int b);

	/**
	 * The context related data container.
	 *
	 * Used to store all context related data.
	 */
	class ContextData: public ContextDataBase {
	public:
		// file for storing allocation rate data
		Plotter::DataFile* file_rate;
		// file for storing allocation count data
		Plotter::DataFile* file_allocs;
		// file for storing deallocation (frees) count data
		Plotter::DataFile* file_frees;

		// total size of allocated resources at the current timestamp
		unsigned int total_size;
		// total number of allocations at the current timestamp
		unsigned int total_allocs;
		// total number of deallocations at the current timestamp
		unsigned int total_frees;

		// allocation/deallocation events occurred during the last time slice
		typedef std::list<event_ptr_t> slice_list_t;
		slice_list_t events;

		/**
		 * Creates a new class instance.
		 * @return
		 */
		ContextData();

		/**
		 * Processes the time slice.
		 *
		 * This method is called when next activity step is calculated.
		 * It removes old events (occurred before the current time slice) and
		 * updates the total_size, total_allocs and total_frees values accordingly.
		 * After the old events are removed the activity step data is written
		 * into data files (file_rate, file_allocs, file_frees) accordingly.
		 * @param[in] timestamp   the time slice end timestamp.
		 * @param[in] slice       the time slice size.
		 */
		void processSlice(unsigned int timestamp, unsigned int slice);

		/**
		 * Adds event to the time slice.
		 *
		 * This method adds event to the time slice event list and updates the
		 * total_size, total_allocs and total_frees values accordingly.
		 * @param[in] event  the event to add.
		 */
		void addEvent(event_ptr_t& event);
	};

	/**
	 * This class stores statistics for the peak values.
	 */
	class Stats {
	public:
		/**
		 * Container for a peak value data.
		 */
		class Data {
		public:
			// number of allocations for allocation/size statistics and
			// number of frees for deallocation statistics
			unsigned int count;
			// total size of allocated resources at the peak time
			unsigned int size;
			// time of the peak value occurrence
			timestamp_t timestamp;
			// file for peak value data
			Plotter::DataFile* file;

			/**
			 * Creates a new class instance.
			 * @return
			 */
			Data() : count(0), size(0), timestamp(0), file(NULL) {
			}
		};

		// peak total resource allocation size
		Data peak_size;
		// peak number of resource allocations
		Data peak_allocs;
		// peak number of resource deallocations (frees)
		Data peak_frees;

		/**
		 * Updates peak value statistics.
		 *
		 * This method is called after each activity step is calculated
		 * to update the peak value statistics.
		 * The peak values are updated only for the main (all) allocation context.
		 * @param[in] cd         the context data containing activity data.
		 * @param[in] timestamp  the activty step timestamp.
		 */
		void update(const ContextData* cd, timestamp_t timestamp);
	};

	/**
	 * The resource related data container
	 */
	class ResourceData: public ResourceDataBase<ContextData> {
	public:
		// resource peak allocation value statistics
		Stats stats;
	};

private:
	// the used resource types
	ReportData<ResourceData> resources;

	/**
	 * Updates Y/Y2 axis range.
	 * @param[in] cd   context data.
	 */
	void updateRangeY(const ContextData* cd);

	/**
	 * Updates X axis range.
	 * @param timestamp
	 */
	void updateRangeX(timestamp_t timestamp);

	/**
	 * Reports occurrence of an event (allocation or deallocation).
	 *
	 * @param[in] resource   the allocated/freed resource type.
	 * @param[in] event      the event.
	 */
	void reportEvent(const Resource* resource, event_ptr_t& event);

	/**
	 * Reports occurance of an event (allocation or deallocation) in
	 * a specific context.
	 *
	 * @param[in] resource   the allocated/freed resource type.
	 * @param[in] context    the context in which the resource is
	 *                       allocated/freed.
	 * @param[in] event      the event.
	 */
	void reportEventInContext(const Resource* resource, const Context* context, event_ptr_t& event);

public:
	// X axis (time) range
	int xrange_min;
	int xrange_max;
	// Y axis (rate) range
	int yrange_min;
	int yrange_max;
	// Y2 axis (allocation/deallocation count) range
	int y2range_max;

	// the timestamp of the current activity step
	timestamp_t activity_step;

	// the activity time slice
	unsigned int slice_value;
	// the activity step
	unsigned int slice_step;

	/**
	 * Creates a new class instance.
	 * @return
	 */
	ActivityGenerator();

	/**
	 * @copydoc ReportGenerator::initialize
	 */
	void initialize();

	/**
	 * @copydoc ReportGenerator::reportAlloc
	 */
	void reportAlloc(const Resource* resource, event_ptr_t& event) {
		reportEvent(resource, event);
	}

	/**
	 * @copydoc ReportGenerator::reportAllocInContext
	 */
	void reportAllocInContext(const Resource* resource, const Context* context, event_ptr_t& event) {
		reportEventInContext(resource, context, event);
	}

	/**
	 * @copydoc ReportGenerator::reportFree
	 */
	void reportFree(const Resource* resource, event_ptr_t& event, event_ptr_t& alloc_event) {
		reportEvent(resource, event);
	}

	/**
	 * @copydoc ReportGenerator::reportFreeInContext
	 */
	void reportFreeInContext(const Resource* resource, const Context* context, event_ptr_t& event, event_ptr_t& alloc_event) {
		reportEventInContext(resource, context, event);
	}

	/**
	 * @copydoc ReportGenerator::reportUnfreedAlloc
	 */
	void reportUnfreedAlloc(const Resource* resource, event_ptr_t& event) {
	}

	/**
	 * @copydoc ReportGenerator::finalize
	 */
	void finalize();
};


#endif
