#ifndef _LIFETIME_GENERATOR_
#define _LIFETIME_GENERATOR_

#include <iostream>
#include <stdexcept>
#include <limits.h>
#include <algorithm>

#include "report_generator.h"
#include "report_data.h"
#include "event.h"
#include "plotter.h"
#include "timeline.h"

class LifetimeGenerator: public ReportGenerator {
private:

	enum {DETAILS_LIMIT = 20};

	class Stats {
	public:
		class Data {
		public:
			// allocation size
			unsigned int size;
			// number of allocation of this size
			unsigned int count;
			// time of the first allocation of this size
			timestamp_t timestamp;

			Data(unsigned int size = 0)
				: size(size), count(0), timestamp(0) {
			}
		};

		Data min;
		Data max;

		Stats()
			: min(INT_MAX) {
		}
	};

	class ResourceData: public ResourceDataBase<int> {
	public:
		typedef std::list<Plotter::DataFile*> events_t;
		typedef std::map<context_t, Plotter::DataFile*> contexts_t;
		typedef std::vector<int> allocs_t;

		Stats stats;
		events_t event_files;
		contexts_t context_files;
		unsigned int total;
		allocs_t allocs;

		unsigned int overhead;
		Plotter::DataFile* file_overhead;

		ResourceData() : overhead(0), file_overhead(NULL) {
		}
	};

	void registerAlloc(ResourceData* rd, event_ptr_t& event, timestamp_t end_timestamp) {
		ResourceData::contexts_t::iterator context_iter = rd->context_files.find(event->context);
		if (context_iter == rd->context_files.end()) {
			std::pair<ResourceData::contexts_t::iterator, bool> pair = rd->context_files.insert(
						ResourceData::contexts_t::value_type(event->context, plotter.createFile(
								Formatter() << rd->key->name << " ({@}" << std::hex << event->context << ")")));
			context_iter = pair.first;
		}
		context_iter->second->write(event->timestamp, event->res_size);
		context_iter->second->write(end_timestamp, event->res_size);
		context_iter->second->writeSeparator();

		if (total_lifelines++ < DETAILS_LIMIT) {
			Plotter::DataFile* file = plotter.createFile(Formatter() << rd->key->name << " (0x" << std::hex << event->res_id << ")");
			file->write(event->timestamp, event->res_size);
			file->write(end_timestamp, event->res_size);
			file->writeSeparator();
			rd->event_files.push_back(file);
		}
	}

	ReportData<ResourceData> resources;

public:
	int xrange_min;
	int xrange_max;
	int yrange_min;
	int yrange_max;

	unsigned int total_lifelines;

	LifetimeGenerator()
		: ReportGenerator("lifetime"), xrange_min(-1), xrange_max(0), yrange_min(0), yrange_max(0), total_lifelines(0) {
	}

	void reportAlloc(const Resource* resource, event_ptr_t& event) {
		ResourceData* rd = resources.getData(resource);

		if (xrange_min == -1) xrange_min = event->timestamp;
		if (xrange_max < event->timestamp) xrange_max = event->timestamp;
		rd->allocs.push_back(event->res_size);
		rd->total += event->res_size;

		if (yrange_max < event->res_size) yrange_max = event->res_size;

		Stats& stats = rd->stats;
		if (event->res_size < stats.min.size) {
			stats.min.size = event->res_size;
			stats.min.count = 0;
			stats.min.timestamp = event->timestamp;
		}
		if (event->res_size == stats.min.size) stats.min.count++;
		if (event->res_size > stats.max.size) {
			stats.max.size = event->res_size;
			stats.max.count = 0;
			stats.max.timestamp = event->timestamp;
		}
		if (event->res_size == stats.max.size) stats.max.count++;
	}

	void reportAllocInContext(const Resource* resource, const Context* context, event_ptr_t& event) {
	}

	void reportFree(const Resource* resource, event_ptr_t& event, event_ptr_t& alloc_event) {
		ResourceData* rd = resources.getData(resource);
		registerAlloc(rd, alloc_event, event->timestamp);
	}

	void reportFreeInContext(const Resource* resource, const Context* context, event_ptr_t& event, event_ptr_t& alloc_event) {
	}

	void reportUnfreedAlloc(const Resource* resource, event_ptr_t& event) {
		ResourceData* rd = resources.getData(resource);
		registerAlloc(rd, event, xrange_max);
	}

	void finalize() {
		if (!yrange_max) {
			throw std::runtime_error("Either the input file does not contain any events "
			                     	 "or no events are matching the specified filter.");
		}
		// increase Y range, so top graph isn't hidden beyond the axis
		yrange_max = yrange_max * 105 / 100;

		// Adjust X axis range to contain at least single point
		if (xrange_max == xrange_min) xrange_max++;
		// number of graphs
		int ngraphs = 0;

		for (ResourceData* rd = resources.first(); rd; rd = resources.next()) {
			// add graphs to the plot
			if (total_lifelines <= DETAILS_LIMIT) {
				for (ResourceData::events_t::iterator iter = rd->event_files.begin(); iter != rd->event_files.end(); iter++) {
					plotter.addGraph(*iter, "1", "2", "column(2)");
					ngraphs++;
				}
			}
			else {
				for (ResourceData::contexts_t::iterator iter = rd->context_files.begin(); iter != rd->context_files.end(); iter++) {
					plotter.addGraph(iter->second, "1", "2", "column(2)");
					ngraphs++;
				}
			}
			// draw statistics markers
			std::sort(rd->allocs.begin(), rd->allocs.end());
			unsigned int alloc_count = rd->allocs.size();
			if (alloc_count) {
				unsigned int stat_average = rd->total / alloc_count;
				Plotter::DataFile* file = plotter.createFile(Formatter() << rd->key->name << "(average:" << stat_average << ")");
				file->write(xrange_min, stat_average);
				file->write(xrange_max, stat_average);
				plotter.addGraph(file,  "1", "2", "column(2)");

				unsigned int stat_median = rd->allocs[alloc_count / 2];
				if (! (alloc_count & 1) ) stat_median = (stat_median + rd->allocs[alloc_count / 2 + 1]) / 2;
				file = plotter.createFile(Formatter() << rd->key->name << "(median:" << stat_median << ")");
				file->write(xrange_min, stat_median);
				file->write(xrange_max, stat_median);
				plotter.addGraph(file,  "1", "2", "column(2)");
			}
		}
		// setup the gnuplot
		plotter.setTitle("Resource life-time");
		plotter.setAxisX("time (secs)", xrange_min, xrange_max);
		plotter.setAxisY("size", yrange_min, yrange_max, "%.1s%c");

		if (total_lifelines > DETAILS_LIMIT) plotter.setStyle("data lines");
		else plotter.setStyle("data linespoints");

		// define table headers
		Plotter::Table* table = plotter.createTable(1, 1);
		// resource name column
		table->addColumn(10);
		// snapshot name
		table->addColumn(5);
		// allocation count
		table->addColumn(8);
		// allocation size
		table->addColumn(10);
		// timestamp
		table->addColumn(12);

		table->setText(0, 0, "Resource", Plotter::Label::ALIGN_CENTER);
		table->setText(0, 1, "State", Plotter::Label::ALIGN_CENTER);

		table->setText(0, 2, "Size", Plotter::Label::ALIGN_CENTER);
		table->setText(0, 3, "Count", Plotter::Label::ALIGN_CENTER);
		table->setText(0, 4, "Time", Plotter::Label::ALIGN_CENTER);

		// write statistics data
		int resource_index = 2;
		for (ResourceData* rd = resources.first(); rd; rd = resources.next()) {
			Stats& stats = rd->stats;
			table->setText(resource_index, 0, rd->key->name, Plotter::Label::ALIGN_LEFT);
			table->setText(resource_index, 1, "min", Plotter::Label::ALIGN_CENTER);
			table->setText(resource_index, 2, Formatter() << stats.min.size);
			table->setText(resource_index, 3, Formatter() << stats.min.count);
			table->setText(resource_index, 4, Timestamp::toString(stats.min.timestamp));
			resource_index += 1;

			table->setText(resource_index, 1, "max", Plotter::Label::ALIGN_CENTER);
			table->setText(resource_index, 2, Formatter() << stats.max.size);
			table->setText(resource_index, 3, Formatter() << stats.max.count);
			table->setText(resource_index, 4, Timestamp::toString(stats.max.timestamp));
			resource_index += 2;
		}
		// Reserve space at the bottom for statistics data
		int bmargin = ngraphs + 9;
		if (bmargin < 9 + ngraphs) bmargin = 9 + ngraphs;
		if (bmargin < 15) bmargin = 15;

		// if the legend contains too many graphs - write it at the left side instead of the bottom.
		if (total_lifelines < DETAILS_LIMIT and bmargin < total_lifelines + 5) {
			plotter.setKey("right outside");
		}
		else {
			plotter.setKey("bmargin");
		}
		plotter.setBMargin(bmargin);
	}
};






#endif

