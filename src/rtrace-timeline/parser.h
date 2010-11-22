#ifndef _PARSER_H_
#define _PARSER_H_

#include "timeline.h"

class Processor;

/**
 * Parses sp-rtrace text log.
 * 
 * This class parses sp-rtrace text log and reports
 * registered resources/contexts and resource allocations/frees.
 */
class Parser {
private:
	/**
	 * Parses function call record.
	 * 
	 * This method parses function call record and registers
	 * the event in processor if successful.
	 * @param[in] buffer   the line to parse.
	 * @return             true if the line was parsed successfully.
	 */
	bool parseCallRecord(const char* buffer);

	/**
	 * Parses context registry record.
	 * 
	 * This method parses context registry record and registers
	 * the context in processor if successful.
	 * @param[in] buffer   the line to parse.
	 * @return             true if the line was parsed successfully.
	 */
	bool parseContextRecord(const char* buffer);

	/**
	 * Parses resource registry record.
	 * 
	 * This method parses resource registry record and registers
	 * the resource type in processor if successful.
	 * @param[in] buffer   the line to parse.
	 * @return             true if the line was parsed successfully.
	 */
	bool parseResourceRecord(const char* buffer);

	// the processor
	Processor* processor;
public:

	/**
	 * Parses the sp-rtrace log file.
	 * 
	 * @param[in] filename   the file to parse.
	 * @param[in] processor  the event processing processor.
	 */
	void parseFile(const std::string& filename, Processor* processor);

};


#endif
