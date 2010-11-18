
#include <exception>
#include <iostream>
#include <tr1/memory>
#include <fstream>
#include <stdexcept>

#include "options.h"

#include "report_data.h"
#include "processor.h"
#include "resource_registry.h"
#include "parser.h"
#include "timestamp.h"


int main(int argc, char* const argv[])
{
	try {
		Processor processor;
		Options::getInstance()->parseCommandLine(argc, argv, &processor);

		if (processor.getGeneratorCount() == 0) {
			throw std::runtime_error("No report type specified");
		}
		processor.initialize();

		Parser parser;
		parser.parseFile(Options::getInstance()->getInFilename(), &processor);
		processor.flushResourceRegistries();
		processor.finalize();
	}
	catch (std::ifstream::failure e) {
		std::cerr << "File error: " << e.what() << "\n";
	}
	catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
	}
	return 0;
}
