#include "processor.h"

#include "formatter.h"
#include "filter_manager.h"
#include "event.h"

void Processor::cleanup() {
	for (generator_list_t::iterator gen_iter = generators.begin(); gen_iter != generators.end(); gen_iter++) {
		gen_iter->get()->cleanup();
	}
}

void Processor::registerResource(int type_id, const std::string& name, bool ref_counted) {
	resource_map_t::iterator iter = resource_registry.find(name);
	if (iter == resource_registry.end()) {
		std::pair<resource_map_t::iterator, bool> pair =
				resource_registry.insert(resource_map_t::value_type(name, resource_registry_ptr_t(new ResourceRegistry(type_id, name, ref_counted))));
	}
	else {
		// TODO: duplicate resources found, throw an error ?
	}
}

void Processor::registerContext(context_t value, const std::string& name) {
	context_map_t::iterator iter = context_registry.find(value);
	if (iter == context_registry.end()) {
		std::pair<context_map_t::iterator, bool> pair =
				context_registry.insert(context_map_t::value_type(value, context_ptr_t(new Context(value, name))));
	}
	else {
		// TODO: duplicate contexts found, throw an error ?
	}
}

void Processor::registerAlloc(int index, context_t context, timestamp_t timestamp,
					const char* res_type, resource_id_t res_id, size_t res_size) {
	resource_map_t::iterator iter = res_type ? resource_registry.find(res_type) : resource_registry.begin();
	if (iter == resource_registry.end()) {
		throw std::runtime_error(Formatter() << "Unknown resource type: " << res_type);
	}
	ResourceRegistry* registry = iter->second.get();
	// apply resource filter
	const std::string& resource_filter = Options::getInstance()->getResourceFilter();
	if (!resource_filter.empty() && resource_filter != registry->resource.name) return;
	// create new event
	event_ptr_t event(new EventAlloc(index, context, timestamp, res_id, res_size));
	// validate event upon specified filters
	if (!FilterManager::getInstance()->validate(event.get())) return;

	// register event in local cache
	event_ptr_t old_event;
	int rc = registry->registerAlloc(event, old_event);
	// TODO: FIRE_EXISTS is returned if an event with the same id was already registered
	// probably would be good to show some warning message or something.
	if (rc > 0) {
		for (generator_list_t::iterator iter = generators.begin(); iter != generators.end(); iter++) {
			ReportGenerator* generator = iter->get();
			// report the generic allocation event
			generator->reportAlloc(&registry->resource, event);
			if (!context_registry.empty()) {
				// report the allocation event in matching contexts.
				for (context_map_t::iterator ctx_iter = context_registry.begin(); ctx_iter != context_registry.end(); ctx_iter++) {
					if (ctx_iter->second.get()->id & context) {
						generator->reportAllocInContext(&registry->resource, ctx_iter->second.get(), event);
					}
				}
				if (!context) generator->reportAllocInContext(&registry->resource, &context_none, event);
			}
		}
	}
}

void Processor::registerFree(int index, context_t context, timestamp_t timestamp,
					const char* res_type, resource_id_t res_id) {
	resource_map_t::iterator iter = res_type ? resource_registry.find(res_type) : resource_registry.begin();
	if (iter == resource_registry.end()) {
		throw std::runtime_error(Formatter() << "Unknown resource type: " << res_type);
	}
	ResourceRegistry* registry = iter->second.get();
	// apply resource filter
	const std::string& resource_filter = Options::getInstance()->getResourceFilter();
	if (!resource_filter.empty() && resource_filter != registry->resource.name) return;

	event_ptr_t event(new EventFree(index, context, timestamp, res_id, 0));
	// validate event upon specified filters
	if (!FilterManager::getInstance()->validate(event.get())) return;

	// find the allocation event for the resource to be freed in local cache.
	event_ptr_t alloc_event;
	int rc = registry->registerFree(event, alloc_event);
	if (rc != ResourceRegistry::BLOCK_SCOPE) {
		event->res_size = alloc_event->res_size;
	}
	// only process deallocation events that are done for resources allocated in our scope
	if (rc > 0) {
		for (generator_list_t::iterator iter = generators.begin(); iter != generators.end(); iter++) {
			ReportGenerator* generator = iter->get();
			// report the generic allocation event
			generator->reportFree(&registry->resource, event, alloc_event);
			if (!context_registry.empty()) {
				// report the allocation event in matching contexts.
				for (context_map_t::iterator ctx_iter = context_registry.begin(); ctx_iter != context_registry.end(); ctx_iter++) {
					if (ctx_iter->second.get()->id & context) {
						generator->reportFreeInContext(&registry->resource, ctx_iter->second.get(), event, alloc_event);
					}
				}
				if (!context) generator->reportFreeInContext(&registry->resource, &context_none, event, alloc_event);
			}
		}
	}
}

void Processor::flushEventCache() {
	for (resource_map_t::iterator iter = resource_registry.begin(); iter != resource_registry.end(); iter++) {
		ResourceRegistry* registry = iter->second.get();
		for (ResourceRegistry::event_map_t::iterator event_iter = registry->events.begin();
				event_iter != registry->events.end(); event_iter++) {
			for (generator_list_t::iterator gen_iter = generators.begin(); gen_iter != generators.end(); gen_iter++) {
				gen_iter->get()->reportUnfreedAlloc(&registry->resource, event_iter->second);
			}
		}
	}
}

void Processor::initialize() {
	for (generator_list_t::iterator gen_iter = generators.begin(); gen_iter != generators.end(); gen_iter++) {
		gen_iter->get()->init();
	}
}


void Processor::finalize() {
	for (generator_list_t::iterator gen_iter = generators.begin(); gen_iter != generators.end(); gen_iter++) {
		gen_iter->get()->finish();
	}
}
