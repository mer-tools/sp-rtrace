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
