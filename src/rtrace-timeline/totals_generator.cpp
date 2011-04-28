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
#include "totals_generator.h"

#include "timestamp.h"
#include "tic_writer.h"


class TicWriter : public ITicWriter {
private:
	timestamp_t start;
	std::vector<timestamp_t>& allocs;
	std::vector<timestamp_t>::iterator iter;
	unsigned int total_count;
public:
	TicWriter(std::vector<timestamp_t>& allocs) :
		start(0),
		allocs(allocs),
		total_count(0) {
		iter = allocs.begin();
	}

	 void write(std::string& output, timestamp_t tic, Plotter::Tic& step) {
		if (!start) start = tic;
		while (iter != allocs.end() && *iter <= tic) {
			total_count++;
			iter++;
		}
		output = Formatter() << total_count << " allocs\\n+" << Timestamp::offsetToString(tic - start);
	}
};

int TotalsGenerator::reportAlloc(const Resource* resource, event_ptr_t& event) {
	if (event->timestamp == 0) {
		fprintf(stderr, "WARNING: Totals report requires input log to have timestamps. Aborting totals report generation.\n");
		return ABORT;
	}
	ResourceData* rd = resources.getData(resource);
	if (resource->overhead && !rd->file_overhead) {
		// new resource found, create a new overhead data file
		rd->file_overhead = plotter.createFile(Formatter() << resource->name << " overhead");
	}
	if (!rd->file_total_allocs) {
		rd->file_total_allocs = plotter.createFile(Formatter() << resource->name << ": total allocs");
	}
	reportAllocInContext(resource, &context_all, event);

	// update statistics
	Stats* stats = &rd->stats;
	stats->end_leaks.add(event->res_size);
	stats->end_totals.add(event->res_size);
	if (stats->end_leaks.size > stats->peak_leaks.size) {
		stats->peak_leaks = stats->end_leaks;
		stats->peak_totals = stats->end_totals;
		stats->peak_timestamp = event->timestamp;
	}
	if (resource->overhead) {
		rd->overhead += event->res_size + resource->overhead;
		rd->file_overhead->write(event->timestamp, rd->overhead);
		if (rd->overhead > yrange_max) yrange_max = rd->overhead;
	}
	rd->total_allocs++;
	rd->file_total_allocs->write(event->timestamp, rd->total_allocs);
	if (rd->total_allocs > y2range_max) y2range_max = rd->total_allocs;

	alloc_timestamps.push_back(event->timestamp);
	return OK;
}

int TotalsGenerator::reportAllocInContext(const Resource* resource, const Context* context, event_ptr_t& event) {
	ResourceData* rd = resources.getData(resource);
	ContextData* cd = rd->getData(context);
	if (!cd->file_totals) {
		// new context found, create a new context data file
		cd->file_totals = plotter.createFile(resource->name);
	}

	// update allocation data
	cd->total += event->res_size;
	cd->file_totals->write(event->timestamp, cd->total);

	// upate X axis range
	if (xrange_min == (unsigned int)-1) xrange_min = event->timestamp;
	if (xrange_max < event->timestamp) xrange_max = event->timestamp;
	// update Y axis range
	if (cd->total > yrange_max) yrange_max = cd->total;
	return OK;
}

int TotalsGenerator::reportFree(const Resource* resource, event_ptr_t& event, event_ptr_t& alloc_event) {
	ResourceData* rd = resources.getData(resource);
	// The free event is reported only for the resources allocated in the scope. Hence
	// the resource data container and its data file will always be created here.
	reportFreeInContext(resource, &context_all, event, alloc_event);

	// update statistics
	Stats* stats = &rd->stats;
	stats->end_leaks.remove(event->res_size);

	if (resource->overhead) {
		rd->overhead -= (event->res_size + resource->overhead);
		rd->file_overhead->write(event->timestamp, rd->overhead);
	}
	return OK;
}

int TotalsGenerator::reportFreeInContext(const Resource* resource, const Context* context, event_ptr_t& event, event_ptr_t& alloc_event) {
	ResourceData* rd = resources.getData(resource);
	ContextData* cd = rd->getData(context);
	if (!cd->file_totals) {
		// new context found, create a new context data file.
		// It might happen if a resource was allocated in one context and freed in another.
		cd->file_totals = plotter.createFile(resource->name);
	}
	// update allocation data
	cd->total -= event->res_size;
	cd->file_totals->write(event->timestamp, cd->total);
	return OK;
}

void TotalsGenerator::finalize() {
	// first check if enough data was gathered
	if (!yrange_max) {
		throw std::runtime_error("Either the input file does not contain any events "
		                     	 "or no events are matching the specified filter.");
	}
	// increase Y range, so top graph isn't hidden beyond the axis
	yrange_max = yrange_max * 105 / 100;
	y2range_max = y2range_max * 105 / 100;
	// number of graphs not counting overhead data
	unsigned int ngraphs = 0;

	// add the collected data to the graph
	for (ResourceData* rd = resources.first(); rd; rd = resources.next()) {
		for (ContextData* cd = rd->first(); cd; cd = rd->next()) {
			plotter.addGraph(cd->file_totals, "1", "2", "column(2)");
			ngraphs++;
		}
		if (rd->file_overhead) {
			plotter.addGraph(rd->file_overhead, "1", "2", "column(2)");
			ngraphs++;
		}
		if (rd->file_total_allocs) {
			plotter.addGraph(rd->file_total_allocs, "1", "2", "column(2)", "x1y2");
			ngraphs++;
		}
		// draw the peak marker
		timestamp_t timestamp = rd->stats.peak_timestamp;
		Plotter::DataFile* file = plotter.createFile(Formatter() << rd->key->name << "(peak:" << Timestamp::toString(timestamp) << ")");
		file->write(timestamp, yrange_min);
		file->write(timestamp, yrange_max);
		plotter.addGraph(file, "1", "2", "column(2)");
	}
	plotter.setTitle("Amount of non-freed allocations");

	TicWriter tic_writer(alloc_timestamps);
	plotter.setAxisX("time (secs) / allocation count", xrange_min, xrange_max, -1, &tic_writer);
	plotter.setAxisY("size", yrange_min, yrange_max, "%.1s%c");
	plotter.setAxisY2("total allocations", yrange_min, y2range_max);
	plotter.setStyle("data lines");

	// Write summary report
	Plotter::Table* table = plotter.createTable(1, 1);
	// resource name column
	table->addColumn(10);
	// snapshot name column
	table->addColumn(5);
	// total allocation count
	table->addColumn(8);
	// total allocation size
	table->addColumn(10);
	// leaked allocation count
	table->addColumn(8);
	// leaked allocation size
	table->addColumn(10);

	table->setText(1, 0, "Resource", Plotter::Label::ALIGN_CENTER);
	table->setText(1, 1, "State", Plotter::Label::ALIGN_CENTER);

	table->setText(0, 2, "Total", Plotter::Label::ALIGN_CENTER);
	table->setText(0, 3, "Total", Plotter::Label::ALIGN_CENTER);
	table->setText(1, 2, "count", Plotter::Label::ALIGN_CENTER);
	table->setText(1, 3, "size", Plotter::Label::ALIGN_CENTER);

	table->setText(0, 4, "Non-freed", Plotter::Label::ALIGN_CENTER);
	table->setText(0, 5, "Non-freed", Plotter::Label::ALIGN_CENTER);
	table->setText(1, 4, "count", Plotter::Label::ALIGN_CENTER);
	table->setText(1, 5, "size", Plotter::Label::ALIGN_CENTER);


	// write summary data
	unsigned int resource_index = 3;
	for (ResourceData* rd = resources.first(); rd; rd = resources.next()) {
		Stats& stats = rd->stats;
		table->setText(resource_index, 0, rd->key->name, Plotter::Label::ALIGN_LEFT);
		table->setText(resource_index, 1, "peak", Plotter::Label::ALIGN_CENTER);
		table->setText(resource_index, 2, Formatter() << stats.peak_totals.count);
		table->setText(resource_index, 3, Formatter() << stats.peak_totals.size);
		table->setText(resource_index, 4, Formatter() << stats.peak_leaks.count);
		table->setText(resource_index, 5, Formatter() << stats.peak_leaks.size);

		resource_index += 1;
		table->setText(resource_index, 1, "end", Plotter::Label::ALIGN_CENTER);
		table->setText(resource_index, 2, Formatter() << stats.end_totals.count);
		table->setText(resource_index, 3, Formatter() << stats.end_totals.size);
		table->setText(resource_index, 4, Formatter() << stats.end_leaks.count);
		table->setText(resource_index, 5, Formatter() << stats.end_leaks.size);

		resource_index += 2;
	}

	// Reserve space at the bottom for leak data
	unsigned int bmargin = ngraphs + 9;
	if (bmargin < 9 + table->rows) bmargin = 9 + table->rows;
	if (bmargin < 15) bmargin = 15;
	plotter.setBMargin(bmargin);
	plotter.setKey("bmargin");
}
