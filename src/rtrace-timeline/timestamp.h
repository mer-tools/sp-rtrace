#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

#include <string>
#include <iomanip>

#include <stdlib.h>
#include <ctype.h>

#include "formatter.h"
#include "timeline.h"


class Timestamp {
public:
	static std::string toString(int hours, int decimal = 3) {
		int msecs = hours % 1000;
		hours /= 1000;
		int seconds = hours % 60;
		hours /= 60;
		int minutes = hours % 60;
		hours /= 60;

		std::ostringstream text;
		text << std::setfill('0') << std::setw(2) << hours << ":" <<  std::setw(2) << minutes << ":" <<  std::setw(2) << seconds;
		if (decimal) {
			for (int dec = decimal; dec < 3; dec++) {
				msecs /= 10;
			}
			text << "." << std::setfill('0') << std::setw(decimal) << msecs;
		}
		return text.str();
	}

	static std::string offsetToString(int hours) {
		int msecs = hours % 1000;
		hours /= 1000;
		int seconds = hours % 60;
		hours /= 60;
		int minutes = hours % 60;
		hours /= 60;

		std::ostringstream text;
		if (hours) text << hours << ":";
		if (text.tellp() != 0) text << std::setfill('0') << std::setw(2);
		if (minutes || text.tellp() != 0) text << minutes << ":";
		if (text.tellp() != 0) text << std::setfill('0') << std::setw(2);
		text << seconds;
		if (msecs) {
			int decimal = 3;
			while (msecs % 10 == 0) {
				decimal--;
				msecs /= 10;
			}
			text << "." << std::setfill('0') << std::setw(decimal) << msecs;
		}
		return text.str();
	}

	static timestamp_t fromString(const std::string& text) {
		timestamp_t timestamp = 0;
		int lpos = text.size();
		int mpos = text.find('.');
		if (mpos != std::string::npos) {
			timestamp = atoi(text.substr(mpos + 1, lpos - mpos - 1).c_str());
			lpos = mpos - 1;
		}
		mpos = text.rfind(':', lpos);
		if (mpos != std::string::npos) {
			timestamp += 1000 * atoi(text.substr(mpos + 1, lpos - mpos).c_str());
			lpos = mpos - 1;
		}
		mpos = text.rfind(':', lpos);
		if (mpos != std::string::npos) {
			timestamp += 1000 * 60 * atoi(text.substr(mpos + 1, lpos - mpos).c_str());
			lpos = mpos - 1;
		}
		timestamp += 1000 * 60 * 60 * atoi(text.substr(0, lpos + 1).c_str());
		return timestamp;
	}
};

#endif
