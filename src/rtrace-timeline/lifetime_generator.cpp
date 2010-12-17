#include "lifetime_generator.h"

#include "timeline.h"

#include "plotter.h"
#include "timestamp.h"

void LifetimeGenerator::registerLifeline(ResourceData* rd, event_ptr_t& event, timestamp_t end_timestamp) {
	// We don't know how many lifetimes will the final report contain. So prepare data for
	// both resolutions - context and resource. The right report resolution will be chosen
	// at the drawing time.
	
	ResourceData::contexts_t::iterator context_iter = rd->context_files.find(event->context);
	if (context_iter == rd->context_files.end()) {
		// new allocation context, create data container for it
		std::pair<ResourceData::contexts_t::iterator, bool> pair = rd->context_files.insert(
					ResourceData::contexts_t::value_type(event->context, plotter.createFile(
							Formatter() << rd->key->name << " (\\\\@" << std::hex << event->context << ")")));
		context_iter = pair.first;
	}
	// write lifetime data for context resolution
	context_iter->second->write(event->timestamp, event->res_size);
	context_iter->second->write(end_timestamp, event->res_size);
	context_iter->second->writeSeparator();

	// if total lifelines exceed the resource resolution limit there are no point of preparing
	// resource resolution data, as the context resolution will be used anyway.
	if (total_lifelines++ < DETAILS_LIMIT) {
		// write lifetime data for resource resolution.
		Plotter::DataFile* file = plotter.createFile(Formatter() << rd->key->name << " (0x" << std::hex << event->res_id << ")");
		file->write(event->timestamp, event->res_size);
		file->write(end_timestamp, event->res_size);
		file->writeSeparator();
		rd->event_files.push_back(file);
	}
}


void LifetimeGenerator::reportAlloc(const Resource* resource, event_ptr_t& event) {
	ResourceData* rd = resources.getData(resource);

	// update X axis range
	if (xrange_min == -1) xrange_min = event->timestamp;
	if (xrange_max < event->timestamp) xrange_max = event->timestamp;
	// update Y axis range
	if (yrange_max < event->res_size) yrange_max = event->res_size;
	
	// register allocation size for median value calculation
	rd->allocs.push_back(event->res_size);
	rd->total += event->res_size;
	
	// update min/max size statistics.
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


void LifetimeGenerator::finalize() {
	// check if the report contains enough data to draw
	if (!yrange_max) {
		throw std::runtime_error("Either the input file does not contain any events "
		                     	 "or no events are matching the specified filter.");
	}
	// increase Y range, so top graph isn't hidden beyond the axis
	yrange_max = yrange_max * 105 / 100;
	// Adjust X axis range to contain at least single point
	if (xrange_max == xrange_min) xrange_max++;
	// number of graphs in report.
	int ngraphs = 0;

	for (ResourceData* rd = resources.first(); rd; rd = resources.next()) {
		// add graphs to the plot. This is the point where we must decide which resolution (context or resource)
		// must be used.
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
		// draw statistics markers (horizontal lines marking average and median allocation values)
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
	if (atoi(Options::getInstance()->getLogscaleSize().c_str()) > 1) plotter.setLogscale("y", Options::getInstance()->getLogscaleSize());

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
