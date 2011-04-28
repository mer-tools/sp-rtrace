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
#include "plotter.h"

#include "options.h"
#include "common/formatter.h"
#include "terminal.h"
#include "timestamp.h"
#include "tic_writer.h"

unsigned int Plotter::DataFile::index = 0;

Plotter::DataFile::DataFile(const std::string& title) {
	name = Formatter() << Options::getInstance()->getWorkingDir() << "/timeline_" << index++ << ".dat";
	file.open(name.c_str());
	if (file.fail()) throw std::ios_base::failure(Formatter() << "Failed to create plotter data file file: " << name);
	if (!title.empty()) file << "Resource \""<< title << "\"\n";
}

void Plotter::Label::write(std::ostream& file, int row, int col, int width) {
	static const char* align_values[3] = {"left", "center", "right"};
	switch (align) {
		case ALIGN_RIGHT:
			col += width - 1;
			break;

		case ALIGN_CENTER:
			col += width / 2;
			break;
	}
	file << "set label \"" << text << "\" at character " << col << "," << row << " " << align_values[align] << "\n";
}

void Plotter::Table::Column::setCell(unsigned int row, const std::string& text, int align) {
	if (cells.size() <= row) cells.resize(row + 1);
	cells[row] = label_ptr_t(new Label(text, align));
}

int Plotter::Table::Column::write(std::ostream& file, unsigned int rows, int offset) {
	unsigned int row = 0;
	for (cells_t::iterator iter = cells.begin(); iter < cells.end(); iter++) {
		if (iter->get()) {
			iter->get()->write(file, rows - row, offset, width);
		}
		row++;
	}
	return width;
}

void Plotter::Table::setText(unsigned int row, unsigned int col, const std::string& text, int align) {
	if (col >= columns.size()) throw std::runtime_error(Formatter() << "Table column number " << col <<
			" out of range (" << columns.size() << ")");

	columns[col]->setCell(row, text, align);
	if (row > rows) rows = row;
}

void Plotter::Table::write(std::ostream& file) {
	int offset = col;
	for (columns_t::iterator iter = columns.begin(); iter != columns.end(); iter++) {
		offset += iter->get()->write(file, rows + row, offset);
	}
}

void Plotter::initialize(const std::string& id) {
	this->id = id;

	config_filename = Formatter() << Options::getInstance()->getWorkingDir() << "/timeline-" << id << ".cfg";
	config.open(config_filename.c_str());
	if (config.fail()) {
		throw std::ios_base::failure(Formatter() << "Failed to create plotter configuration file: " << config_filename);
	}

	config << Options::getInstance()->getTerminal()->getHeader();
}

void Plotter::cleanup() {
	for (datafiles_t::iterator file_iter = files.begin(); file_iter != files.end(); file_iter++) {
		file_iter->get()->remove();
	}
	remove(config_filename.c_str());
}

void Plotter::plot(int fd_out) {
	// writes the table data
	for (tables_t::iterator table_iter = tables.begin(); table_iter != tables.end(); table_iter++) {
		table_iter->get()->write(config);
	}
	
	// closes data files
	for (datafiles_t::iterator file_iter = files.begin(); file_iter != files.end(); file_iter++) {
		file_iter->get()->close();
	}
	
	// writes the graphs
	config << "plot \\\n";
	for (graphs_t::iterator graph_iter = graphs.begin(); graph_iter != graphs.end(); graph_iter++) {
		config << *graph_iter;
	}
	config << "\n";

	// writes terminal footer
	config << Options::getInstance()->getTerminal()->getFooter();

	config.close();

	// invokes gnuplot to create output report
	int status, pid = fork();
	if (pid == 0) {
		dup2(fd_out, STDOUT_FILENO);
		execlp("gnuplot", "gnuplot", config_filename.c_str(), NULL);
		exit(errno);
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
		throw std::runtime_error(Formatter() << "Failed to generate report with gnuplot (" <<
				strerror(WEXITSTATUS(status)) << "). Check if the gnuplot is installed.");
	}
}


void Plotter::setTitle(const std::string& title) {

	config << "set title \"" << title;
	const std::string& filter = Options::getInstance()->getFilterDescription();
	if (filter != "") {
		config << "\\n(" << filter << ")";
	}
	config << "\"\n";
}

void Plotter::setStyle(const std::string& style) {
	config << "set style " << style << "\n";
}

void Plotter::setAxisX(const std::string& label, int min, int max, int scale, ITicWriter* tic_writer) {
	config << "set xtics rotate nomirror\n";

	if (scale != -1) {
		config << "set xtics scale " << scale << "\n";
	}

	if (min != -1) {
		std::auto_ptr<ITicWriter> default_writer;
		if (!tic_writer) {
			default_writer = std::auto_ptr<ITicWriter>(new DefaultTicWriter());
			tic_writer = default_writer.get();
		}

		if (min == max) max++;
		int range = max - min;
		Tic step((double)range / 10 + 0.5);
		int tic = min - min % step.value;
		//if (min % step.value > 0) tic -= step.value;
		min = tic;

		// set label
		config << "set xlabel \"" << label << "\" offset 0,0\n";
		// set range
		config << "set xrange[" << min << ":" << max << "]\n";
		// place autotics outside range to avoid interference with manual tics
		config << "set xtics " << max * 2 << "," << max * 2 << "\n";
		// write tics
		std::string stic;
		while (tic <= max - step.value) {
			tic_writer->write(stic, tic, step);
			config << "set xtics add (\"" << stic << "\" " << tic << ")\n";
			tic += step.value;
		}
		// write the ending tic
		tic_writer->write(stic, max, step);
		config << "set xtics add (\"" << stic << "\" " << max << ")\n";
	}

}

void Plotter::setAxisY(const std::string& label, int min, int max, const std::string& format) {
	if (min == max) max++;
	config << "set yrange[" << min << ":" << max << "]\n";
	if (!format.empty()) config << "set format y \"" << format << "\"\n";
	config << "set ytics out\n";
	config << "set ylabel \"" << label << "\"\n";
}

void Plotter::setAxisY2(const std::string& label, int min, int max, const std::string& format) {
	if (min == max) max++;
	config << "set y2range[" << min << ":" << max << "]\n";
	if (!format.empty()) config << "set format y2 \"" << format << "\"\n";
	config << "set y2tics out\n";
	config << "set ytics nomirror\n";
	config << "set y2label \"" << label << "\"\n";
}

void Plotter::setBMargin(int value) {
	config << "set bmargin " << value << "\n";
}

int Plotter::setLineStyle(const std::string& type, const std::string& color) {
	++line_style_index;
	config << "set style line " << ++line_style_index;
	if (!type.empty()) config << " lt " << type;
	if (!color.empty()) config << " linecolor rgb \"" << color << "\"";
	config << "\n";
	return line_style_index;
}

void Plotter::setKey(const std::string& key) {
	config << "set key " << key << "\n";
}

void Plotter::setSeparator(const std::string& value) {
	config << "set datafile separator \"" << value << "\"\n";
}

void Plotter::setGrid(const std::string& value) {
	config << "set grid " << value << "\n";
}

Plotter::Table* Plotter::createTable(int row, int col) {
	Table* table = new Table(row, col);
	tables.push_back(table_ptr_t(table));
	return table;
}

Plotter::DataFile* Plotter::createFile(const std::string& title) {
	DataFile* file = new DataFile(title);
	files.push_back(datafile_ptr_t(file));
	return file;
}

void Plotter::addGraph(const Plotter::DataFile* file, const std::string& col_x, const std::string& col_y, const std::string& title,
		const std::string& axis, int style, const std::string& prefix) {
	std::ostringstream data;
	if (graphs.size()) data << ",";
	if (!prefix.empty()) data << prefix << " ";
	data << "\"" << file->getName() << "\" using "<< col_x;
	if (!col_y.empty()) data << ":" << col_y;
	if (style != -1) data << " ls " << style;
	data << " title " << title;
	if (!axis.empty()) data << "axes " << axis;
	data << "\\\n";
	graphs.push_back(data.str());
}

void Plotter::setLogscale(const std::string& axis, const std::string& scale)
{
	config << "set logscale " << axis << " " << scale << "\n";
}

