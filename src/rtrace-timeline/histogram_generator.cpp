#include "histogram_generator.h"

#include "plotter.h"

unsigned int HistogramGenerator::Stats::getMedian() {
	unsigned int nallocs = allocs.size();
	if (!nallocs) return 0;
	unsigned int median = allocs[nallocs / 2];
	if (!(nallocs & 1)) median = (median + allocs[nallocs / 2 + 1]) / 2;
	return median;
}

void HistogramGenerator::reportAlloc(const Resource* resource, event_ptr_t& event) {
	ResourceData* rd = resources.getData(resource);

	// Abort if the input data contains multiple resource types as histogram
	// reports can handle only single resource type.
	if (resource_type) {
		if (resource_type->type_id != resource->type_id) {
			throw std::runtime_error(Formatter() << "ERROR: multiple resources (" << resource_type->name <<
					"," << resource->name << ") detected, the results might be misleading.\n"
					"Use --filter-resource options to filter a single resource.");
		}
	}
	else resource_type = resource;

	// increase total allocation count for the resource size
	ResourceData::data_t::iterator iter = rd->allocs.find(event->res_size);
	if (iter == rd->allocs.end()) {
		// new resource size found - add alloc data for the resource size 
		std::pair<ResourceData::data_t::iterator, bool> pair = rd->allocs.insert(
				ResourceData::data_t::value_type(event->res_size, Alloc()));
		iter = pair.first;
	}
	iter->second.total++;
}

void HistogramGenerator::reportFree(const Resource* resource, event_ptr_t& event, event_ptr_t& alloc_event) {
	ResourceData* rd = resources.getData(resource);
	// free events are reported only for allocated resources. That means the alloc data for this
	// resource will be already created in reportAlloc() method.
	rd->allocs[event->res_size].freed++;
}

void HistogramGenerator::finalize() {
	Stats stats_freed, stats_unfreed, stats_summ;

	ResourceData* rd = resources.first();
	if (!rd) {
		throw std::runtime_error("Either the input file does not contain any events "
		                     	 "or no events are matching the specified filter.");
	}

	// the data in rd->allocs aren't ordered so create size index
	keys_t keys;
	for (ResourceData::data_t::iterator iter = rd->allocs.begin(); iter != rd->allocs.end(); iter++) {
		keys.push_back(iter->first);
	}
	sort(keys.begin(), keys.end());

	// calculate median values
	for (keys_t::iterator iter = keys.begin(); iter != keys.end(); iter++) {
		unsigned int size = *iter;
		Alloc& alloc = rd->allocs[size];
		for (unsigned int i = 0; i < alloc.freed; i++) {
			stats_freed.allocs.push_back(size);
			stats_summ.allocs.push_back(size);
		}
		for (unsigned int i = 0; i < alloc.total - alloc.freed; i++) {
			stats_unfreed.allocs.push_back(size);
			stats_summ.allocs.push_back(size);
		}

	}
	// create data file
	Plotter::DataFile* file = plotter.createFile();

	// create line styles for freed and unfreed bars
	int lt_unfreed = plotter.setLineStyle("", "#FF0000");
	int lt_freed = plotter.setLineStyle("", "#00FF00");

	// Histogram data starts with second column (the first column contains
	// allocation size).
	file->writeText("size\n");
	plotter.addGraph(file, "2", "xtic(1)", Formatter() << "\"" << rd->key->name << " allocation " <<
				mode << " (unfreed)\"", "", lt_unfreed, "newhistogram \"Resource size\", ");

	plotter.addGraph(file, "3", "xtic(1)", Formatter() << "\"" << rd->key->name << " allocation " <<
				mode << " (freed)\"", "", lt_freed);

	// write data
	for (keys_t::iterator iter = keys.begin(); iter != keys.end(); iter++) {
		unsigned int size = *iter;
		file->writeText(Formatter() << size);
		ResourceData::data_t::iterator iter = rd->allocs.find(size);
		if (iter == rd->allocs.end()) {
			file->writeText("\t-\t-");
		}
		else {
			Alloc& alloc = iter->second;
			writeAlloc(file, alloc, size);

			// calculate statistics
			unsigned int unfreed = alloc.total - alloc.freed;
			stats_freed.count += alloc.freed;
			stats_freed.total += alloc.freed * size;

			stats_unfreed.count += unfreed;
			stats_unfreed.total += unfreed * size;
		}
		file->writeSeparator();
	}
	// setup gnuplot
	plotter.setTitle(title);
	plotter.setAxisY(ylabel, 0, yrange_max, "%.1s%c");
	plotter.setGrid("ytics");
	plotter.setAxisX("Resource size", -1, -1, 0);
	plotter.setStyle("data histogram");
	plotter.setStyle("histogram rowstacked");
	plotter.setStyle("fill solid 0.2");
	plotter.setSeparator("\\t");

	// write statistics table
	Plotter::Table* table = plotter.createTable(1, 1);

	// name column
	table->addColumn(10);
	// median
	table->addColumn(12);
	// allocation count
	table->addColumn(10);
	// allocation size
	table->addColumn(12);

	table->setText(0, 0, "Allocs", Plotter::Label::ALIGN_CENTER);
	table->setText(0, 1, "Median", Plotter::Label::ALIGN_CENTER);
	table->setText(0, 2, "Count", Plotter::Label::ALIGN_CENTER);
	table->setText(0, 3, "Total Size", Plotter::Label::ALIGN_CENTER);

	table->setText(2, 0, "freed");
	table->setText(3, 0, "unfreed");
	table->setText(4, 0, "both");

	table->setText(2, 1, Formatter() << stats_freed.getMedian());
	table->setText(3, 1, Formatter() << stats_unfreed.getMedian());
	table->setText(4, 1, Formatter() << stats_summ.getMedian());

	table->setText(2, 2, Formatter() << stats_freed.count);
	table->setText(3, 2, Formatter() << stats_unfreed.count);
	table->setText(4, 2, Formatter() << stats_summ.count);

	table->setText(2, 3, Formatter() << stats_freed.total);
	table->setText(3, 3, Formatter() << stats_unfreed.total);
	table->setText(4, 3, Formatter() << stats_summ.total);

	plotter.setBMargin(10);
	plotter.setKey("invert");
	plotter.setKey("bmargin");
}


void SizeHistogramGenerator::writeAlloc(Plotter::DataFile* file, Alloc& alloc, unsigned int size) {
	unsigned int unfreed = alloc.total - alloc.freed;
	file->writeText(Formatter() << "\t" << size * unfreed << "\t" << size * alloc.freed);
	if (yrange_max < size * alloc.total) yrange_max = size * alloc.total;
}

void CountHistogramGenerator::writeAlloc(Plotter::DataFile* file, Alloc& alloc, unsigned int size) {
	unsigned int unfreed = alloc.total - alloc.freed;
	file->writeText(Formatter() << "\t" << unfreed << "\t" << alloc.freed);
	if (yrange_max < alloc.total) yrange_max = alloc.total;
}
