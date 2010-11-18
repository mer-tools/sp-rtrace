#ifndef _PARSER_H_
#define _PARSER_H_

#include <string>

class Processor;

class Parser {
private:
	bool parseCallRecord(const char* buffer);

	bool parseContextRecord(const char* buffer);

	bool parseResourceRecord(const char* buffer);

	Processor* processor;
public:

	void parseFile(const std::string& filename, Processor* processor);

};


#endif
