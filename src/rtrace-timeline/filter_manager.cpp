#include "filter_manager.h"

bool FilterManager::validate(const Event* event) {
	for (filters_t::iterator iter = filters.begin(); iter != filters.end(); iter++) {
		if (!iter->get()->validate(event)) return false;
	}
	return true;
}
