/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2011 by Nokia Corporation
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

#ifndef _GENERATOR_DATA_H_
#define _GENERATOR_DATA_H_

#include "timeline.h"
#include "event.h"

/*
 * Base classes for containers used to store intermediate reporter data.
 *
 * The reports must be able to store separate data for different resources
 * and contexts. It's implemented as map inside map: data[resource][context].
 * The following classes provide abstract base for common data storage
 * implementation.
 *
 * Declaration:
 * 1) Create class for context data storage derived from ContextDataBase:
 *      class ContextData: public ContextDataBase
 * 2) Create class for resource data storage derived from ResourceDataBase
 *    with the created ContextData class as template parameter:
 *      class ResourceData: public ResourceDataBase<ContextData>
 * 3) Declare data storage object of ReportData class with the created
 *    ResourceData class as template parameter:
 *      ReportData<ResourceData> resources
 *
 * Usage (statistics gathering):
 * 1) Get the resource data for the specified Resource. The resource data
 *    is created if necessary (for the first getData() call for resource):
 *      ResourceData* rd = resources.getData(resource)
 * 2) Get the context data for the specified Context. The context data
 *    is created if necessary (for the first getData() call for context):
 *      ContextData* cd = rd->getData(context)
 *
 * Usage (statistics processing):
 * 1) Iterate through stored resource data:
 *      for (ResourceData* rd = resources.first(); rd; rd = resources.next())
 * 2) Access the associated resource with rd->key (key type is Resource*).
 * 3) Iterate through stored context data:
 *        for (ContextData* cd = rd->first(); cd; cd = rd->next())
 * 4) Access the associated context with cd->key (key type is Context*).
 */

/*
 * Index mapping classes, used to map the index classes
 * (Resource, Context) to index type and index fields in them.
 *
 */
template <class _Tval>
class AbstractMapKey {
};

template<>
class AbstractMapKey<Context> {
public:
	// the data type used as context index (map key value)
	typedef context_t key_t;
	/**
	 * Retrieves the index field of the Context class instance.
	 * @param context
	 * @return
	 */
	static key_t queryId(const Context* context) {
		return context->id;
	}
};

template<>
class AbstractMapKey<Resource> {
public:
	// the data type used as resource index (map key value)
	typedef resource_type_t key_t;
	/**
	 * Retrieves the index field of the Resource class instance.
	 * @param resource
	 * @return
	 */
	static key_t queryId(const Resource* resource) {
		return resource->type_id;
	}
};

/**
 * The AbstracMap class provides access to _Tdata object by the
 * specified index key. If the map doesn't have _Tdata object
 * associated with the specified key, a new object is created
 * and stored into map.
 * The AbstractMap class also provides methods to iterated
 * through the stored data.
 */
template <class _Tkey, class _Tdata>
class AbstractMap {
protected:
	typedef std::tr1::shared_ptr<_Tdata> data_ptr_t;
	typedef std::map<typename AbstractMapKey<_Tkey>::key_t, data_ptr_t> data_map_t;

	data_map_t _data;
	typename data_map_t::iterator _iter;

	_Tdata* getIteratorValue() {
		return _iter == _data.end() ? NULL : _iter->second.get();
	}

public:

	_Tdata* getData(const _Tkey* key) {
		typename data_map_t::iterator iter = _data.find(AbstractMapKey<_Tkey>::queryId(key));
		if (iter != _data.end()) {
			return iter->second.get();
		}
		std::pair<typename data_map_t::iterator, bool> pair = _data.insert(
					typename data_map_t::value_type(AbstractMapKey<_Tkey>::queryId(key), data_ptr_t(new _Tdata)));
		pair.first->second->key = key;
		return pair.first->second.get();
	}

	_Tdata* first() {
		_iter = _data.begin();
		return getIteratorValue();
	}

	_Tdata* next() {
		_iter++;
		return getIteratorValue();
	}
};

/**
 * The AbstractData class stores reference to the index object
 * so it can be retrieved during iteration (first/next).
 */
template <class _Tkey>
class AbstractData {
public:
	const _Tkey* key;
};

/**
 * Context data base class.
 */
class ContextDataBase: public AbstractData<Context> {
};

/**
 * Resource data base class.
 */
template <class _Tcd>
class ResourceDataBase: public AbstractMap<Context, _Tcd>, public AbstractData<Resource> {
public:
};

/**
 * The ReportData class provides storage of data indexed by resources and contexts.
 */
template <class _Trd>
class ReportData: public AbstractMap<Resource, _Trd> {
};



#endif
