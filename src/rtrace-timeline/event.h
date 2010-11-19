#ifndef _EVENT_H_
#define _EVENT_H_

#include "timeline.h"

/**
 * Base class for allocation/deallocation events.
 */
class Event {
public:

	/* event type enumeration */
	enum {NONE, ALLOC, FREE};

public:
	// call record index
	int index;
	// reference counter. Used to track reference counted resources
	int ref_count;
	// allocation/deallocation call context
	context_t context;
	// event timestamp
	timestamp_t timestamp;
	// allocated resource size for allocation events, 0 for deallocation events.
	size_t res_size;
	// allocated/freed resource identifier
	resource_id_t res_id;
	// even type (ALLOC, FREE)
	unsigned int type;

public:

	/**
	 * Creates a new class instance.
	 *
	 * @param[in] type      the event type (ALLOC, FREE).
	 * @param[in] index     the call record index.
	 * @param[in] context   the call context mask.
	 * @param[in] timestamp the call timestamp.
	 * @param[in] res_id    the allocated/freed resource identifier.
	 * @param[in] res_size  the allocated resource size (ALLOC events) or
	 *                      0 (FREE events).
	 */
	Event(unsigned int type, int index, context_t context, timestamp_t timestamp, resource_id_t res_id, size_t res_size) :
		type(type), index(index), context(context), timestamp(timestamp), res_id(res_id), res_size(res_size) {
	}

};

/**
 * Resource allocation event.
 */
class EventAlloc: public Event {
public:

	/**
	 * Creates a new class instance.
	 *
	 * @param[in] index     the call record index.
	 * @param[in] context   the call context mask.
	 * @param[in] timestamp the call timestamp.
	 * @param[in] res_id    the allocated/freed resource identifier.
	 * @param[in] res_size  the allocated resource size (ALLOC events) or
	 *                      0 (FREE events).
	 */
	EventAlloc(int index, context_t context, timestamp_t timestamp, resource_id_t res_id, size_t res_size) :
		Event(ALLOC, index, context, timestamp, res_id, res_size) {
	}

};

/**
 * Resource deallocation (free) event.
 */
class EventFree: public Event {
public:

	/**
	 * Creates a new class instance.
	 *
	 * @param[in] index     the call record index.
	 * @param[in] context   the call context mask.
	 * @param[in] timestamp the call timestamp.
	 * @param[in] res_id    the allocated/freed resource identifier.
	 * @param[in] res_size  the allocated resource size (ALLOC events) or
	 *                      0 (FREE events).
	 */
	EventFree(int index, context_t context, timestamp_t timestamp, resource_id_t res_id, size_t res_size) :
		Event(FREE, index, context, timestamp, res_id, res_size) {
	}

};

typedef std::tr1::shared_ptr<Event> event_ptr_t;

/**
 * Resource type implementation.
 */
class Resource {
public:
	// resource type identifier
	resource_type_t type_id;
	// resource type name
	std::string name;
	// true if resource is reference counted
	bool ref_counted;
	// resource allocation overhead value
	unsigned int overhead;

	/**
	 * Creates a new class instance.
	 *
	 * @param[in] type_id     the resource type identifier.
	 * @param[in] name        the resource type name.
	 * @param[in] ref_counted true if resource is reference counted.
	 * @param[in] overhead    the resource allocation overhead value.
	 * @return
	 */
	Resource(resource_type_t type_id, const std::string& name, bool ref_counted, unsigned int overhead = 0)
		: type_id(type_id), name(name), ref_counted(ref_counted), overhead(overhead) {
	}
};

/**
 * Call context implementation.
 */
class Context {
public:
	// context identifier (mask)
	context_t id;
	// context name
	std::string name;

	/**
	 * Creates a new class instance.
	 *
	 * @param[in] id     the context identifier (mask).
	 * @param[in] name   the context name.
	 * @return
	 */
	Context(context_t id, const std::string& name)
		: id(id), name(name) {
	}
};

typedef std::tr1::shared_ptr<Context> context_ptr_t;

#endif

