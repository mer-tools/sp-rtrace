#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <tr1/memory>
#include <string>
#include <map>

#include "timeline.h"
#include "event.h"

class ResourceRegistry {
public:

	typedef std::map<resource_id_t, event_ptr_t> event_map_t;

	typedef std::map<std::string, unsigned int> overhead_map_t;
	static overhead_map_t overhead_map;

	static unsigned int getResourceOverhead(const std::string& res_type) {
		overhead_map_t::iterator iter = overhead_map.find(res_type);
		if (iter == overhead_map.end()) return 0;
		return iter->second;
	}

public:

	enum {BLOCK_SCOPE = -1,
		  BLOCK_REFCOUNT = -2,
		  REPORT_OK = 1,
		  REPORT_EXISTS = 2
		};

	Resource resource;

	event_map_t events;

	ResourceRegistry(int type_id, const std::string& name, bool ref_counted)
		: resource(type_id, name, ref_counted, ResourceRegistry::getResourceOverhead(name) ) {
	}

	int registerAlloc(event_ptr_t& event, event_ptr_t& old_event) {
		event_map_t::iterator iter = events.find(event->res_id);
		if (iter == events.end()) {
			events.insert(event_map_t::value_type(event->res_id, event));
			return REPORT_OK;
		}
		if (resource.ref_counted) {
			iter->second->ref_count++;
			return BLOCK_REFCOUNT;
		}
		old_event = iter->second;
		events[event->res_id] = event;
		return REPORT_EXISTS;
	}

	int registerFree(event_ptr_t& event, event_ptr_t& alloc_event) {
		event_map_t::iterator iter = events.find(event->res_id);
		if (iter == events.end()) return BLOCK_SCOPE;
		if (resource.ref_counted && --iter->second->ref_count > 0) return BLOCK_REFCOUNT;
		alloc_event = iter->second;
		events.erase(iter);
		return REPORT_OK;
	}



};

typedef std::tr1::shared_ptr<ResourceRegistry> resource_registry_ptr_t;


#endif
