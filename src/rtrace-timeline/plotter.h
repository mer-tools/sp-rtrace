#ifndef _PLOTTER_H_
#define _PLOTTER_H_

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <tr1/memory>
#include <string>
#include <fstream>
#include <vector>
#include <list>
#include <stdexcept>

#include "formatter.h"
#include "terminal.h"
#include "options.h"
#include "timestamp.h"

class Plotter {
public:

	class DataFile {
	private:
		std::ofstream file;
		std::string name;

		static unsigned int index;
	public:
		DataFile(const std::string& title) {
			name = Formatter() << "timeline_" << index++ << ".dat";
			file.open(name.c_str());
			if (file.fail()) throw std::ios_base::failure(Formatter() << "Failed to create plotter data file file: " << name);
			if (!title.empty()) file << "Resource \""<< title << "\"\n";
		}

		const std::string& getName() const {
			return name;
		}

		void write(int x, int y) {
			file << x << " " << y << "\n";
		}

		void writeSeparator() {
			file << "\n";
		}

		void writeText(const std::string& text) {
			file << text;
		}

		void close() {
			file.close();
		}

		void remove() {
			::remove(name.c_str());
		}
	};
	typedef std::tr1::shared_ptr<DataFile> datafile_ptr_t;


	class Label {
	public:
		enum {ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT};

		const std::string text;
		int align;

		Label(const std::string& text, int align = ALIGN_RIGHT)
			: text(text), align(align) {
		}

		void write(std::ostream& file, int row, int col, int width) {
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
	};
	typedef std::tr1::shared_ptr<Label> label_ptr_t;

	class Table {
	public:
		class Column {
		private:
			int width;
			typedef std::vector<label_ptr_t> cells_t;
			cells_t cells;
		public:
			Column(int width)
				: width(width) {
			}

			void setCell(int row, const std::string& text, int align) {
				if (cells.size() <= row) cells.resize(row + 1);
				cells[row] = label_ptr_t(new Label(text, align));
			}

			int write(std::ostream& file, int rows, int offset) {
				int row = 0;
				for (cells_t::iterator iter = cells.begin(); iter < cells.end(); iter++) {
					if (iter->get()) {
						iter->get()->write(file, rows - row, offset, width);
					}
					row++;
				}
				return width;
			}
		};
		typedef std::tr1::shared_ptr<Column> column_ptr_t;

		int rows;
		int row;
		int col;
		typedef std::vector<column_ptr_t> columns_t;
		columns_t columns;

		Table(int row, int col)
			: row(row), col(col), rows(0) {
		}

		void addColumn(int width) {
			columns.push_back(column_ptr_t(new Column(width)));
		}

		void setText(int row, int col, const std::string& text, int align = Label::ALIGN_RIGHT) {
			if (col >= columns.size()) throw std::runtime_error(Formatter() << "Table column number " << col <<
					" out of range (" << columns.size() << ")");

			columns[col]->setCell(row, text, align);
			if (row > rows) rows = row;
		}

		void write(std::ostream& file) {
			int offset = col;
			for (columns_t::iterator iter = columns.begin(); iter != columns.end(); iter++) {
				offset += iter->get()->write(file, rows + row, offset);
			}
		}
	};
	typedef std::tr1::shared_ptr<Table> table_ptr_t;

private:

	typedef std::list<table_ptr_t> tables_t;
	tables_t tables;

	typedef std::list<datafile_ptr_t> datafiles_t;
	datafiles_t files;

	typedef std::list<std::string> graphs_t;
	graphs_t graphs;

	std::string id;
	std::ofstream config;

	std::string config_filename;

	int line_style_index;
public:

	class Tic {
	public:
		int value;
		int decimal;

		Tic(int slice, bool rounded = true)
			: value(1), decimal(3) {
			if (rounded) {
				int round = 1;
				int round_value = slice;
				while (round_value) {
					value = round_value * round;
					round *= 10;
					round_value /= 10;
				}
			}
			else {
				value = slice;
			}
			slice = value;
			while (decimal && slice && slice % 10 == 0) {
				decimal--;
				slice /= 10;
			}
		}

	};

public:

	Plotter()
		: line_style_index(0) {
	}

	void initialize(const std::string& id) {
		this->id = id;

		config_filename = Formatter() << "timeline-" << id << ".cfg";
		config.open(config_filename.c_str());
		if (config.fail()) {
			throw std::ios_base::failure(Formatter() << "Failed to create plotter configuration file: " << config_filename);
		}

		config << Options::getInstance()->getTerminal()->getHeader();
	}

	void cleanup() {
		// TODO: uncomment when done debugging.

		for (datafiles_t::iterator file_iter = files.begin(); file_iter != files.end(); file_iter++) {
//			file_iter->get()->remove();
		}

		//	remove(config_filename.c_str());
	}

	void plot(int fd_out) {
		for (tables_t::iterator table_iter = tables.begin(); table_iter != tables.end(); table_iter++) {
			table_iter->get()->write(config);
		}

		for (datafiles_t::iterator file_iter = files.begin(); file_iter != files.end(); file_iter++) {
			file_iter->get()->close();
		}

		config << "plot \\\n";
		for (graphs_t::iterator graph_iter = graphs.begin(); graph_iter != graphs.end(); graph_iter++) {
			config << *graph_iter;
		}
		config << "\n";

		config << Options::getInstance()->getTerminal()->getFooter();

		config.close();

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

	void setTitle(const std::string& title) {

		config << "set title \"" << title;
		const std::string& filter = Options::getInstance()->getFilterDescription();
		if (filter != "") {
			config << "\\n(" << filter << ")";
		}
		config << "\"\n";
	}

	void setStyle(const std::string& style) {
		config << "set style " << style << "\n";
	}

	void setAxisX(const std::string& label, int min = -1, int max = -1, const std::string& format = "", int scale = -1) {
		config << "set xtics rotate nomirror\n";

		if (scale != -1) {
			config << "set xtics scale " << scale << "\n";
		}

		if (min != -1) {
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
			// place tics
			while (tic <= max - step.value) {
				config << "set xtics add (\"" << Timestamp::toString(tic, step.decimal) << "\\n+" <<
						Timestamp::offsetToString(tic - min) << "\" " << tic << ")\n";
				tic += step.value;
			}
			config << "set xtics add (\"" << Timestamp::toString(max) << "\\n+" <<
					Timestamp::offsetToString(range) << "\" " << max << ")\n";
		}

	}

	void setAxisY(const std::string& label, int min, int max, const std::string& format = "") {
		if (min == max) max++;
		config << "set yrange[" << min << ":" << max << "]\n";
		if (!format.empty()) config << "set format y \"" << format << "\"\n";
		config << "set ytics out\n";
		config << "set ylabel \"" << label << "\"\n";
	}

	void setAxisY2(const std::string& label, int min, int max, const std::string& format = "") {
		if (min == max) max++;
		config << "set y2range[" << min << ":" << max << "]\n";
		if (!format.empty()) config << "set format y2 \"" << format << "\"\n";
		config << "set y2tics out\n";
		config << "set ytics nomirror\n";
		config << "set y2label \"" << label << "\"\n";
	}

	void setBMargin(int value) {
		config << "set bmargin " << value << "\n";
	}

	int setLineStyle(const std::string& type, const std::string& color) {
		++line_style_index;
		config << "set style line " << ++line_style_index;
		if (!type.empty()) config << " lt " << type;
		if (!color.empty()) config << " linecolor rgb \"" << color << "\"";
		config << "\n";
		return line_style_index;
	}

	void setKey(const std::string& key) {
		config << "set key " << key << "\n";
	}

	void setSeparator(const std::string& value) {
		config << "set datafile separator \"" << value << "\"\n";
	}

	void setGrid(const std::string& value) {
		config << "set grid " << value << "\n";
	}

	Table* createTable(int row, int col) {
		Table* table = new Table(row, col);
		tables.push_back(table_ptr_t(table));
		return table;
	}

	DataFile* createFile(const std::string& title = "") {
		DataFile* file = new DataFile(title);
		files.push_back(datafile_ptr_t(file));
		return file;
	}

	void addGraph(const DataFile* file, const std::string& col_x, const std::string& col_y, const std::string& title = "",
			const std::string& axis = "", int style = -1, const std::string& prefix = "") {
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
};

#endif

