#include "filter.h"

SizeFilter::SizeFilter(const std::string& text) {
	size = atoi(text.c_str());
	if (size) {
		int mod = text[text.size() - 1];
		if (mod == 'K' || mod == 'k') {
			size *= 1024;
		}
		else if (mod == 'M' || mod == 'm') {
			size *= 1024 * 1024;
		}
	}
}

IndexFilter::IndexFilter(const std::string& text) {
	index = atoi(text.c_str());
	if (index) {
		int mod = text[text.size() - 1];
		if (mod == 'K' || mod == 'k') {
			index *= 1000;
		}
		else if (mod == 'M' || mod == 'm') {
			index *= 1000 * 1000;
		}
	}
}

ContextFilter::ContextFilter(const std::string& text)
	: context_mask(0) {
	std::stringstream stream(text);
	stream >> std::hex >> context_mask;
};
