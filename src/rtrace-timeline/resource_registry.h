#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "timeline.h"
#include "event.h"

/**
 * The allocation event registry for a resource type.
 * 
 * All allocation events of the specific resource are
 * registered into registry to enable allocation event
 * lookups for deallocation events later.
 */
class ResourceRegistry {
public:

	typedef std::map<resource_id_t, event_ptr_t> event_map_t;

	// resource allocation overhead data.
	typedef std::map<std::string, unsigned int> overhead_map_t;
	static overhead_map_t overhead_map;

	
	static unsigned int getResourceOverhead(const std::string& res_type) {
		overhead_map_t::iterator iter = overhead_map.find(res_type);
		if (iter == overhead_map.end()) return 0;
		return iter->second;
	}

public:

	/**
	 * Return codes of registerXX methods.
	 *
	 * The codes are divided into BLOCK and REPORT codes.
	 * REPORT code instructs caller that the resource was added
	 * to registry and the event must be reported (to report
	 * generators).
	 * BLOCK code means that the event should not be reported
	 * (to generators). Currently there can be two causes - the
	 * resource is reference counted and the event simply changed
	 * its reference counter without really allocating/deallocating
	 * resource, or it was deallocation event for a resource allocated
	 * outside the report scope.
	 */
	enum rc_t {
		  BLOCK_SCOPE = -1,        // event out of scope. Returned from
		                           // registerFree() if cache does not contain allocation
		                           // event for the specified resource

		  BLOCK_REFCOUNT = -2,     // event reference count increased/decreased. Returned
		                           // for reference counted resources when event does not
		                           // allocate or free resource, but just increases its
		                           // reference counter

		  REPORT_OK = 1,           // event registered successfully

		  REPORT_EXISTS = 2        // event for the resource identifier was already registered.
		                           // Returned from registerAlloc() for non reference counted
		                           // resources, if the allocated resource identifier is already
		                           // in the cache
		};

	// the associated resource
	Resource resource;

	// the event cache
	event_map_t events;

	/**
	 * Creates a new class instance.
	 *
	 * @param[in] type_id      the resource type identifier.
	 * @param[in] name         the resource type name.
	 * @param[in] ref_counted  true if the resource is reference counted.
	 */
	ResourceRegistry(int type_id, const std::string& name, bool ref_counted)
		: resource(type_id, name, ref_counted, ResourceRegistry::getResourceOverhead(name) ) {
	}

	/**
	 * Registers allocation event.
	 *
	 * @param[in] event      the event to register.
	 * @param[in] old_event  the old allocation event for the resource id.
	 * @return               the registry return code - see rc_t enumeration.
	 */
	int registerAlloc(event_ptr_t& event, event_ptr_t& old_event);

	/**
	 * Registers deallocation (free) event.
	 *
	 * @param[in] event        the event to register.
	 * @param[in] alloc_event  the allocation event for the resource id to be freed.
	 * @return                 the registry return code - see rc_t enumeration.
	 */
	int registerFree(event_ptr_t& event, event_ptr_t& alloc_event);

};

typedef std::tr1::shared_ptr<ResourceRegistry> resource_registry_ptr_t;


#endif
