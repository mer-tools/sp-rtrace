#include "activity_generator.h"

#include "plotter.h"
#include "timestamp.h"

ActivityGenerator::ContextData::ContextData()
	: file_rate(NULL), file_allocs(NULL), file_frees(NULL),	total_size(0), total_allocs(0), total_frees(0)
{
}

void ActivityGenerator::ContextData::processSlice(unsigned int timestamp, unsigned int slice)
{
	// go through events and remove events outside time slice
	for (slice_list_t::iterator iter = events.begin(); iter != events.end();) {
		event_ptr_t event = *iter;
		// Events are ordered by timestamps - so after finding the first event
		// in slice the job is done.
		if (event->timestamp >= timestamp - slice) break;

		// remove the event from slice and update the slice totals accordingly
		slice_list_t::iterator del_iter = iter++;
		events.erase(del_iter);
		if (event->type == Event::ALLOC) {
			total_size -= event->res_size;
			--total_allocs;
		}
		else if (event->type == Event::FREE) {
			--total_frees;
		}
	}
	// write the step data
	file_rate->write(timestamp, total_size);
	file_allocs->write(timestamp, total_allocs);
	file_frees->write(timestamp, total_frees);
}

void ActivityGenerator::ContextData::addEvent(event_ptr_t& event)
{
	// adds the event to slice and updates totals accordingly
	if (event->type == Event::ALLOC) {
		total_size += event->res_size;
		++total_allocs;
	}
	else if (event->type == Event::FREE) {
		++total_frees;
	}
	events.push_back(event);
}

void ActivityGenerator::Stats::update(const ContextData* cd, timestamp_t timestamp)
{
	if (cd->total_size > peak_size.size) {
		peak_size.size = cd->total_size;
		peak_size.count = cd->total_allocs;
		peak_size.timestamp = timestamp;
	}
	if (cd->total_allocs > peak_allocs.count) {
		peak_allocs.size = cd->total_size;
		peak_allocs.count = cd->total_allocs;
		peak_allocs.timestamp = timestamp;
	}
	if (cd->total_frees > peak_frees.count) {
		peak_frees.count = cd->total_frees;
		peak_frees.size = cd->total_size;
		peak_frees.timestamp = timestamp;
	}
}


void ActivityGenerator::updateRangeY(const ContextData* cd)
{
	// update Y, Y2 axis ranges
	if (cd->total_size > yrange_max) yrange_max = cd->total_size;
	if (cd->total_allocs > y2range_max) y2range_max = cd->total_allocs;
	if (cd->total_frees > y2range_max) y2range_max = cd->total_frees;
}

void ActivityGenerator::updateRangeX(timestamp_t timestamp)
{
	// update X axis range
	if (xrange_min == -1) xrange_min = timestamp;
	if (xrange_max < timestamp) xrange_max = timestamp;
}

int ActivityGenerator::reportEvent(const Resource* resource, event_ptr_t& event)
{
	if (event->timestamp == 0) {
		fprintf(stderr, "WARNING: Activity report requires input log to have timestamps. Aborting activity report generation.\n");
		return ABORT;
	}
	timestamp_t old_slice_timestamp = activity_step;
	reportEventInContext(resource, &context_all, event);

	// update the statistics if the activity step has been changed
	if (old_slice_timestamp != activity_step) {
		ResourceData* rd = resources.getData(resource);
		Stats* stats = &rd->stats;
		rd->stats.update(rd->getData(&context_all), activity_step);
		old_slice_timestamp = activity_step;
	}
	return OK;
}

int ActivityGenerator::reportEventInContext(const Resource* resource, const Context* context, event_ptr_t& event) {
	ResourceData* rd = resources.getData(resource);
	ContextData* cd = rd->getData(context);
	if (!cd->file_allocs) {
		// create data files for new context
		cd->file_rate = plotter.createFile(Formatter() << resource->name << " (rate:" << context->name << ")");
		cd->file_allocs = plotter.createFile(Formatter() << resource->name << " (allocs:" << context->name << ")");
		cd->file_frees = plotter.createFile(Formatter() << resource->name << " (frees:" << context->name << ")");
	}

	if (activity_step == 0) activity_step = event->timestamp;
	timestamp_t timestamp = activity_step + slice_step;
	// process timeslice if the event timestamp is outside it
	if (timestamp <= event->timestamp) {
		cd->processSlice(timestamp, slice_value);
		updateRangeX(timestamp);
		updateRangeY(cd);
		activity_step = timestamp;
	}
	cd->addEvent(event);
	return OK;
}

ActivityGenerator::ActivityGenerator()
	: ReportGenerator("activity"), xrange_min(-1), xrange_max(0), yrange_min(0), yrange_max(0), y2range_max(0),
	  activity_step(0), slice_value(0), slice_step(0) {
}

void ActivityGenerator::initialize()
{
	Plotter::Tic slice(Options::getInstance()->getSlice());
	slice_value = slice.value;
	slice_step = slice_value / 2;
	if (!slice_step) slice_step = 1;
}


void ActivityGenerator::finalize()
{
	// calculate the last slice data
	timestamp_t timestamp = activity_step + slice_step;
	for (ResourceData* rd = resources.first(); rd; rd = resources.next()) {
		for (ContextData* cd = rd->first(); cd; cd = rd->next()) {
			cd->processSlice(timestamp, slice_value);
			updateRangeX(timestamp);
			updateRangeY(cd);
		}
		rd->stats.update(rd->getData(&context_all), timestamp);
	}
	// check if the gathered data is not empty
	if (!yrange_max) {
		throw std::runtime_error("Either the input file does not contain any events "
		                     	 "or no events are matching the specified filter.");
	}
	// increase Y range, so top graph isn't hidden beyond the axis
	yrange_max = yrange_max * 105 / 100;
	y2range_max = y2range_max * 105 / 100;
	// number of graphs not counting overhead data
	int ngraphs = 0;

	// add graphs to the plot
	for (ResourceData* rd = resources.first(); rd; rd = resources.next()) {
		for (ContextData* cd = rd->first(); cd; cd = rd->next()) {
			plotter.addGraph(cd->file_rate, "1", "2", "column(2)");
			plotter.addGraph(cd->file_allocs, "1", "2", "column(2)", "x1y2");
			plotter.addGraph(cd->file_frees, "1", "2", "column(2)", "x1y2");
			ngraphs += 3;
		}
		// plot the peak markers (vertical lines at peak value timestamps)
		Plotter::DataFile* file = plotter.createFile(Formatter() << rd->key->name << " (peak rate:" <<
				Timestamp::toString(rd->stats.peak_size.timestamp));
		file->write(rd->stats.peak_size.timestamp, yrange_min);
		file->write(rd->stats.peak_size.timestamp, yrange_max);
		plotter.addGraph(file, "1", "2", "column(2)");

		file = plotter.createFile(Formatter() << rd->key->name << " (peak allocs:" <<
				Timestamp::toString(rd->stats.peak_allocs.timestamp));
		file->write(rd->stats.peak_allocs.timestamp, yrange_min);
		file->write(rd->stats.peak_allocs.timestamp, yrange_max);
		plotter.addGraph(file, "1", "2", "column(2)");

		file = plotter.createFile(Formatter() << rd->key->name << " (peak frees:" <<
				Timestamp::toString(rd->stats.peak_frees.timestamp));
		file->write(rd->stats.peak_frees.timestamp, yrange_min);
		file->write(rd->stats.peak_frees.timestamp, yrange_max);
		plotter.addGraph(file, "1", "2", "column(2)");
	}

	// generate the gnuplot configuration file
	plotter.setTitle("Allocation/deallocation rate");

	plotter.setAxisX("time (secs)", xrange_min, xrange_max);
	std::string slice_format = Formatter() << float(slice_value) / 1000;
	plotter.setAxisY(Formatter() << "amount per " << slice_format << " sec", yrange_min, yrange_max, "%.1s%c");
	plotter.setAxisY2(Formatter() << "count per " << slice_format << " sec", yrange_min, y2range_max);
	plotter.setStyle("data lines");

	Plotter::Table* table = plotter.createTable(1, 1);
	// create table columns
	// resource name column
	table->addColumn(10);
	// snapshot name column
	table->addColumn(10);
	// allocation count
	table->addColumn(8);
	// allocation size
	table->addColumn(10);
	// set column headers
	table->setText(0, 0, "Resource", Plotter::Label::ALIGN_CENTER);
	table->setText(0, 1, "State", Plotter::Label::ALIGN_CENTER);
	table->setText(0, 2, "Count", Plotter::Label::ALIGN_CENTER);
	table->setText(0, 3, "Size", Plotter::Label::ALIGN_CENTER);

	// write summary data
	int resource_index = 2;
	for (ResourceData* rd = resources.first(); rd; rd = resources.next()) {
		Stats& stats = rd->stats;
		table->setText(resource_index, 0, rd->key->name, Plotter::Label::ALIGN_LEFT);
		table->setText(resource_index, 1, "peak size", Plotter::Label::ALIGN_CENTER);
		table->setText(resource_index, 2, Formatter() << stats.peak_size.count);
		table->setText(resource_index, 3, Formatter() << stats.peak_size.size);

		resource_index += 1;
		table->setText(resource_index, 1, "peak allocs", Plotter::Label::ALIGN_CENTER);
		table->setText(resource_index, 2, Formatter() << stats.peak_allocs.count);
		table->setText(resource_index, 3, Formatter() << stats.peak_allocs.size);

		resource_index += 1;
		table->setText(resource_index, 1, "peak frees", Plotter::Label::ALIGN_CENTER);
		table->setText(resource_index, 2, Formatter() << stats.peak_frees.count);
		table->setText(resource_index, 3, Formatter() << stats.peak_frees.size);

		resource_index += 2;
	}

	// Reserve space at the bottom for leak data
	int bmargin = ngraphs + 9;
	if (bmargin < 9 + table->rows) bmargin = 9 + table->rows;
	if (bmargin < 15) bmargin = 15;
	plotter.setBMargin(bmargin);
	plotter.setKey("bmargin");

}
