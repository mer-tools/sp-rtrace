
#include "resource_registry.h"


static const ResourceRegistry::overhead_map_t::value_type overhead_map_data[] = {
		 ResourceRegistry::overhead_map_t::value_type("memory", 8),
};

ResourceRegistry::overhead_map_t ResourceRegistry::overhead_map(overhead_map_data,
			overhead_map_data + sizeof(overhead_map_data) / sizeof(overhead_map_data[0]));
