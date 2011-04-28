/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010,2011 by Nokia Corporation
 *
 * Contact: Eero Tamminen <eero.tamminen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "resource_registry.h"

// define resource allocation overhead data
static const ResourceRegistry::overhead_map_t::value_type overhead_map_data[] = {
		 ResourceRegistry::overhead_map_t::value_type("memory", 8),
};

ResourceRegistry::overhead_map_t ResourceRegistry::overhead_map(overhead_map_data,
			overhead_map_data + sizeof(overhead_map_data) / sizeof(overhead_map_data[0]));

//

int ResourceRegistry::registerAlloc(event_ptr_t& event, event_ptr_t& old_event) {
	event_map_t::iterator iter = events.find(event->res_id);
	if (iter == events.end()) {
		// No registered allocation events for the resource id.
		// Register the event and return success code.
		events.insert(event_map_t::value_type(event->res_id, event));
		return REPORT_OK;
	}
	if (resource.ref_counted) {
		// An allocation event for the resource id has been found.
		// As the resource is reference counted - increase its counter
		// and return block code.
		iter->second->ref_count++;
		return BLOCK_REFCOUNT;
	}
	// A new allocation event for resource id that was already allocated before.
	// Shouldn't happen - so there is something fishy going on. Register the
	// new event and return success code.
	old_event = iter->second;
	events[event->res_id] = event;
	return REPORT_EXISTS;
}

int ResourceRegistry::registerFree(event_ptr_t& event, event_ptr_t& alloc_event) {
	event_map_t::iterator iter = events.find(event->res_id);
	// No registered allocation events for the resource id. Return block code.
	if (iter == events.end()) return BLOCK_SCOPE;
	// Reference counted resource, decrement its counter and
	// return block code if the counter is not yet zero
	if (resource.ref_counted && --iter->second->ref_count > 0) return BLOCK_REFCOUNT;
	// In the other cases remove allocation event from registry and return success code.
	alloc_event = iter->second;
	events.erase(iter);
	return REPORT_OK;
}
