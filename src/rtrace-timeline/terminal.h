#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include "timeline.h"

class TerminalWriter {
public:

	virtual ~TerminalWriter() {}

	virtual std::ostream& write(std::ostream& stream) const = 0;

	friend std::ostream &operator << (std::ostream &stream, const TerminalWriter& writer);
};

class EmptyTerminalWriter: public TerminalWriter {
public:
	virtual std::ostream& write(std::ostream& stream) const {
		return stream;
	}
};


class PngTerminalHeader: public TerminalWriter {
public:

	std::ostream& write(std::ostream& stream) const;
};

class EpsTerminalHeader: public TerminalWriter {
public:

	std::ostream& write(std::ostream& stream) const;
};

class WxtTerminalHeader: public TerminalWriter {
public:

	std::ostream& write(std::ostream& stream) const;
};

class WxtTerminalFooter: public TerminalWriter {
public:

	std::ostream& write(std::ostream& stream) const;
};

class Terminal {
private:
	std::auto_ptr<TerminalWriter> header;
	std::auto_ptr<TerminalWriter> footer;
public:

	Terminal()
		: header(dynamic_cast<TerminalWriter*>(new EmptyTerminalWriter())),
		  footer(dynamic_cast<TerminalWriter*>(new EmptyTerminalWriter())) {
	}

	void setHeader(TerminalWriter* writer) {
		header = std::auto_ptr<TerminalWriter>(writer);
	}

	void setFooter(TerminalWriter* writer) {
		footer = std::auto_ptr<TerminalWriter>(writer);
	}

	const TerminalWriter& getHeader() const {
		return *header.get();
	}

	const TerminalWriter& getFooter() const {
		return *footer.get();
	}

};

#endif
