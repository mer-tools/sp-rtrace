#ifndef _PLOTTER_H_
#define _PLOTTER_H_

#include "timeline.h"

/**
 * Helper class for gnuplot configuration file setup.
 * 
 * As the methods simply are wrappers around gnuplot commands, 
 * see the gnuplot documentation for argument detalized explanation.
 */
class Plotter {
public:

	/**
	 * The gnuplot data file implementation.
	 * 
	 * The gnuplot data files are used to store the plot data.
	 * The files are created and managed by the Plotter.
	 */
	class DataFile {
	private:
		// the data file
		std::ofstream file;
		// the data file name 
		std::string name;
		
		// the internal data file index used for name generation 
		static unsigned int index;
	public:
		/**
		 * Creates a new class instance.
		 * 
		 * @param[in] title    the title describing the data.
		 */
		DataFile(const std::string& title);
		
		/**
		 * Retrieves the file name.
		 * 
		 * @return  the file name.
		 */
		const std::string& getName() const {
			return name;
		}
		
		/**
		 * Writes plot data into file.
		 * 
		 * @param[in] x   the x coordinate.
		 * @param[in] y   the y coordinate.
		 */
		void write(int x, int y) {
			file << x << " " << y << "\n";
		}

		/**
		 * Writes separator to separate data blocks.
		 */
		void writeSeparator() {
			file << "\n";
		}

		/**
		 * Writes plain text into data file.
		 * 
		 * @param[in] text   the text to write.
		 */
		void writeText(const std::string& text) {
			file << text;
		}

		/**
		 * Closes the data file.
		 */
		void close() {
			file.close();
		}

		/**
		 * Deletes the data file.
		 */
		void remove() {
			::remove(name.c_str());
		}
	};
	typedef std::tr1::shared_ptr<DataFile> datafile_ptr_t;


	/**
	 * Gnuplot label implementation.
	 */
	class Label {
	public:
		// the text alignment enum
		enum {ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT};

		// the label text
		const std::string text;
		// the label alignment
		int align;

		/**
		 * Creates a new class instance.
		 * 
		 * @param[in] text    the label text.
		 * @param[in] align   the label alignment.
		 */
		Label(const std::string& text, int align = ALIGN_RIGHT)
			: text(text), align(align) {
		}

		/**
		 * Writes the label data into gnuplot configuration file.
		 * 
		 * @param[in] file   the gnuplot configuration file.
		 * @param[in] row    the label row.
		 * @param[in] col    the label column.
		 * @param[in] width  the label width.
		 */
		void write(std::ostream& file, int row, int col, int width);
	};
	typedef std::tr1::shared_ptr<Label> label_ptr_t;

	/**
	 * Gnuplot table implementation.
	 * 
	 * This class provides table functionality by using gnuplot labels. 
	 */
	class Table {
	public:
		/**
		 * The table column
		 */
		class Column {
		private:
			// column width
			int width;
			// column cells
			typedef std::vector<label_ptr_t> cells_t;
			cells_t cells;
		public:
			/*
			 * Creates a new class instance.
			 * 
			 * @param[in] width   - the column width.
			 */
			Column(int width)
				: width(width) {
			}
			
			/**
			 * Sets column cell.
			 * 
			 * @param[in] row      the cell row.
			 * @param[in] text     the cell text.
			 * @param[in] align    the cell alignment.
			 */
			void setCell(int row, const std::string& text, int align);

			/**
			 * Writes the column data into gnuplot configuration file.
			 * 
			 * @param[in] file       the gnuplot configuration file.
			 * @param[in] rows       the number of rows in table.
			 * @param[in] offset     the offset column.
			 */
			int write(std::ostream& file, int rows, int offset);
		};
		typedef std::tr1::shared_ptr<Column> column_ptr_t;

		// the number of rows in table
		int rows;
		// the row position of the top left table corner
		int row;
		// the column position of the top left table corner
		int col;
		// the table columns
		typedef std::vector<column_ptr_t> columns_t;
		columns_t columns;

		/**
		 * Creates a new class instance.
		 * 
		 * @param[in] row    the row position of the top left table corner.
		 * @param[in] col    the column position of the top left table corner.
		 */
		Table(int row, int col)
			: row(row), col(col), rows(0) {
		}

		/**
		 * Adds a new column to the table.
		 * 
		 * @param[in] wdith   the column width.
		 */
		void addColumn(int width) {
			columns.push_back(column_ptr_t(new Column(width)));
		}

		/**
		 * Sets the table cell.
		 *  
		 * @param[in] row    the cell row.
		 * @param[in] col    the cell column.
		 * @param[in] text   the cell alignment.
		 * @param[in] align  the cell text.
		 */
		void setText(int row, int col, const std::string& text, int align = Label::ALIGN_RIGHT);
		
		/**
		 * Writes the table data into gnuplot configuration file.
		 *  
		 * @param[in] file   the gnuplot configuration file.
		 */
		void write(std::ostream& file);
	};
	typedef std::tr1::shared_ptr<Table> table_ptr_t;

private:

	// the used tables
	typedef std::list<table_ptr_t> tables_t;
	tables_t tables;

	// the used data files
	typedef std::list<datafile_ptr_t> datafiles_t;
	datafiles_t files;

	// the graphs to draw
	typedef std::list<std::string> graphs_t;
	graphs_t graphs;

	// the plot id
	std::string id;
	
	// the configuration file
	std::ofstream config;

	// the configuration file name
	std::string config_filename;

	// the internal line style index 
	int line_style_index;
public:

	/**
	 * The tic mark
	 * 
	 * This class calculates rounded values for tic marks.
	 */
	class Tic {
	public:
		// the tic value
		int value;
		// the number of digits after decimal point
		int decimal;

		/**
		 * Creates a new class instance.
		 * 
		 * @param[in] slice    the requested tic value
		 * @param[in] rounded  true, if the tic value must be rounded.
		 */
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

	/**
	 * Creates a new class instance.
	 */
	Plotter()
		: line_style_index(0) {
	}

	/**
	 * Initializes plotter.
	 * 
	 * This class creates gnuplot configuration file and writes terminal header.
	 * @param[in] id   the plotter identifier.
	 */
	void initialize(const std::string& id);
	
	/**
	 * Performs cleanup.
	 * 
	 * Currently it simply removes the configuration and data files.
	 */
	void cleanup();

	/**
	 * Draws the created tables and graphs.
	 */
	void plot(int fd_out);
	
	/**
	 * Sets the report title.
	 * 
	 * @param[in] title   the report title.
	 */
	void setTitle(const std::string& title);
	
	/**
	 * Sets style option.
	 * 
	 * @param[in] style  the style option to set.
	 */
	void setStyle(const std::string& style);

	/**
	 * Sets X axis.
	 * 
	 * @param[in] label   the label written along the axis.
	 * @param[in] min     the minimal range.
	 * @param[in] max     the maximal range.
	 * @param[in] scale   the tic mark scale.
	 */
	void setAxisX(const std::string& label, int min = -1, int max = -1, int scale = -1);
	
	/**
	 * Sets Y axis
	 * 
	 * @param[in] label   the label written along the axis.
	 * @param[in] min     the minimal range.
	 * @param[in] max     the maximal range.
	 * @param[in] format  the format of tic mark labels.
	 */
	void setAxisY(const std::string& label, int min, int max, const std::string& format = "");
	

	/**
	 * Sets second Y axis.
	 * @param[in] label   the label written along the axis.
	 * @param[in] min     the minimal range.
	 * @param[in] max     the maximal range.
	 * @param[in] format  the format of tic mark labels.
	 */
	void setAxisY2(const std::string& label, int min, int max, const std::string& format = "");

	/**
	 * Sets the bottom margin.
	 * 
	 * @param[in] value  the bottom margin value.
	 */
	void setBMargin(int value);

	/**
	 * Creates a new line style.
	 * 
	 * @param[in] type    the line type, can be empty.
	 * @param[in] color   the line color, can be empty.
	 * @return            the index of the created style.
	 */
	int setLineStyle(const std::string& type, const std::string& color);

	/**
	 * Sets the placement for the graphs legend.
	 * 
	 * @param[in] key   the placemenet documentation.
	 */
	void setKey(const std::string& key);

	/**
	 * Sets separator symbol for data files.
	 * 
	 * By default space is used as data separator. However if the data may contain
	 * spaces - another separator symbol can be set (for example tab).
	 * @param[in] value  the separator symbol.
	 */
	void setSeparator(const std::string& value);

	/**
	 * Sets grid option.
	 *
	 * By default the grid is disabled. 
	 * @param[in] value  the grid to set. For example to set a grid based on y tics
	 *                   use "ytics" as value.
	 */
	void setGrid(const std::string& value);

	/**
	 * Creates gnuplot table.
	 * 
	 * The created tables are managed by Plotter - written with plot() method
	 * and destroyed together with Plotter.
	 * @param[in] row  the table row.
	 * @param[in] col  the table column.
	 * @return         a reference to the created table.
	 */
	Table* createTable(int row, int col);

	/**
	 * Creates gnuplot data file.
	 * 
	 * The data files are managed by Plotter and destroyed/deleted together
	 * with the Plotter.
	 * @param[in] title  the title describing data stored in file.
	 * @return           a reference to the created data file.
	 */
	DataFile* createFile(const std::string& title = "");

	/**
	 * Adds a new graph based on the specified data file.
	 * 
	 * @param[in] file     the gnuplot data file.
	 * @param[in] col_x    the data column in file containing X axis value.
	 * @param[in] col_y    the data column in file containing Y axis value.
	 * @param[in] title    the graph title (can be empty, defaulting to the title in data file). 
	 * @param[in] axis     the graph Y axis (can be empty, defaulting to Y axis).
	 * @param[in] style    the graph style (can be empty, assigned automatically).
	 * @param[in] prefix   the graph prefix (used only for histogram reports).
	 */
	void addGraph(const DataFile* file, const std::string& col_x, const std::string& col_y, const std::string& title = "",
			const std::string& axis = "", int style = -1, const std::string& prefix = "");
};

#endif

