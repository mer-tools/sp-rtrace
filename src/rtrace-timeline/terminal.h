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
#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include "timeline.h"

/**
 * Base class for terminal headers/footers.
 *
 * Gnuplot output terminals have different initialization
 * commands and some of them can have 'finishing' command.
 * Those commands are written with terminal headers and
 * footers, based on TerminalWriter class.
 */
class TerminalWriter {
public:

	/**
	 * Destroys the object.
	 * @return
	 */
	virtual ~TerminalWriter() {}

	/**
	 * Writes the terminal data into the stream.
	 *
	 * @param[in] stream  the output stream (gnuplot configuration file).
	 */
	virtual std::ostream& write(std::ostream& stream) const = 0;


	friend std::ostream &operator << (std::ostream &stream, const TerminalWriter& writer);
};

/**
 * Empty terminal writer.
 */
class EmptyTerminalWriter: public TerminalWriter {
public:
	/**
	 * @copydoc TerminalWriter::write
	 */
	virtual std::ostream& write(std::ostream& stream) const {
		return stream;
	}
};


/**
 * Png terminal header.
 */
class PngTerminalHeader: public TerminalWriter {
public:

	/**
	 * @copydoc TerminalWriter::write
	 */
	std::ostream& write(std::ostream& stream) const;
};

/**
 * Eps (postscript) terminal header
 */
class EpsTerminalHeader: public TerminalWriter {
public:

	/**
	 * @copydoc TerminalWriter::write
	 */
	std::ostream& write(std::ostream& stream) const;
};

/**
 * Wxt (wxWidget window) terminal header
 */
class WxtTerminalHeader: public TerminalWriter {
public:

	/**
	 * @copydoc TerminalWriter::write
	 */
	std::ostream& write(std::ostream& stream) const;
};

/**
 * Wxt (wxWidget window) terminal footer.
 */
class WxtTerminalFooter: public TerminalWriter {
public:

	/**
	 * @copydoc TerminalWriter::write
	 */
	std::ostream& write(std::ostream& stream) const;
};

/**
 * The terminal class manages gnuplot terminal footers and
 * headers.
 */
class Terminal {
private:
	// the terminal header
	std::auto_ptr<TerminalWriter> header;
	// the terminal footer
	std::auto_ptr<TerminalWriter> footer;
public:

	/**
	 * Creates a new class instance.
	 *
	 * By default initialize headers with empty writers.
	 * @return
	 */
	Terminal()
		: header(dynamic_cast<TerminalWriter*>(new EmptyTerminalWriter())),
		  footer(dynamic_cast<TerminalWriter*>(new EmptyTerminalWriter())) {
	}

	/**
	 * Sets a new terminal header.
	 *
	 * The terminal manages the passed writer object and it will
	 * be destroyed together with the terminal.
	 * @param[in] writer  the new header writer.
	 */
	void setHeader(TerminalWriter* writer) {
		header = std::auto_ptr<TerminalWriter>(writer);
	}

	/**
	 * Sets a new terminal footer.
	 *
	 * The terminal manages the passed writer object and it will
	 * be destroyed together with the terminal.
	 * @param[in] writer  the new footer writer.
	 */
	void setFooter(TerminalWriter* writer) {
		footer = std::auto_ptr<TerminalWriter>(writer);
	}

	/**
	 * Retrieves the terminal header.
	 *
	 * @return   the terminal header writer.
	 */
	const TerminalWriter& getHeader() const {
		return *header.get();
	}

	/**
	 * Retrieves the terminal footer.
	 *
	 * @return   the terminal footer writer.
	 */
	const TerminalWriter& getFooter() const {
		return *footer.get();
	}
};

#endif
