#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include "terminal.h"

class Processor;

class Options {
private:
	// the output file template
	std::string out_template;
	// the input file name
	std::string in_filename;
	// the output file extension
	std::string out_extension;

	// the output image scaling factor
	unsigned int scale_x;
	unsigned int scale_y;
	// the time slice for activity report (milliseconds)
	unsigned int slice;

	// the output terminal
	Terminal terminal;
	
	// the resource filter value
	std::string resource_filter;
	
	// description of the specified filters
	std::string filter_desc;

	
	/**
	 * Displays application usage instructions.
	 */
	void displayUsage();


	/**
	 * Splits filter option range value into min/max components.
	 *
	 * @param[in] range  the range value in format <min>-<max>.
	 * @param[out] min   the minimum range value.
	 * @param[out] max   the maximum range value.
	 */
	void splitRange(const char* range, std::string& min, std::string& max);

	/**
	 * Updates filter description.
	 *
	 * This method updates filter description string which is displayed
	 * in the second line of the report titles.
	 * @param[in] name   the filter name.
	 * @param[in] value  the filter value.
	 */
	void updateFilterDesc(const std::string& name, const std::string& value);

public:

	/**
	 * Creates a new class instance.
	 */
	Options();

	/**
	 * Returns the options object instance.
	 * 
	 * @return   the Options instance.
	 */
	static Options* getInstance() {
		static Options instance;
		return &instance;
	}
	

	/**
	 * Parses command line arguments.
	 *
	 * @param argc
	 * @param argv
	 * @param[in] processor   the log processor. 
	 */
	void parseCommandLine(int argc, char* const argv[], Processor* processor);

	/**
	 * Retrieves output file name template.
	 *
	 * This template is used by generators to construct the
	 * output file name. Usually it has the following format:
	 * <output filename template>-<genrator id>.<file type extension>
	 * @return   the output file name template.
	 */
	const std::string& getOutTemplate() const {
		return out_template;
	}


	/**
	 * Retrieves input file name.
	 * @return  the input file name.
	 */
	const std::string& getInFilename() const {
		return in_filename;
	}

	/**
	 * Retrieves output filename extension.
	 *
	 * The output filename extension depends on the used terminal type.
	 * @return
	 */
	const std::string& getOutExtension() const {
		return out_extension;
	}

	/**
	 * Retrieves horizontal scaling factor.
	 *
	 * The horizontal scaling factor affects output report width.
	 * @return    the horizontal scaling factor in percents.
	 */
	unsigned int getScaleX() const {
		return scale_x;
	}

	/**
	 * Retrieves vertical scaling factor.
	 *
	 * The vertical scaling factor affects output report height.
	 * @return    the vertical scaling factor in percents.
	 */
	unsigned int getScaleY() const {
		return scale_y;
	}

	/**
	 * Retrieves the current terminal.
	 *
	 * @return
	 */
	const Terminal* getTerminal() const {
		return &terminal;
	}

	/**
	 * Retrieves short description of active filters.
	 *
	 * @return   the description of active filters.
	 */
	const std::string getFilterDescription() const {
		return filter_desc;
	}

	/**
	 * Retrieves the activity report time slice setting.
	 * 
	 * @return   the time slice for activity report.
	 */
	unsigned int getSlice() const {
		return slice;
	}

	/**
	 * Retrieves resource filter value.
	 * 
	 * @return   the resource filter.
	 */
	const std::string& getResourceFilter() const {
		return resource_filter;
	}

};


#endif
