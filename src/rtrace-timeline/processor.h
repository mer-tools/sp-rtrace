#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include "timeline.h"
#include "report_generator.h"
#include "resource_registry.h"

/**
 * The event processor.
 * 
 * This class manages context/resource/function events received from parser.
 * The resource and context data are stored in the processor while function
 * call events are reported further to the report generators.
 * The function call reporting is done in the following order:
 * 1) find the associated resource type, throw exception if none has been registered.
 * 2) check if the resource passes resource filter (if one specified)
 * 3) create event object based on the reported data
 * 4) validate the created event against event filters.
 * 5.allocs) register events in local cache
 * 5.frees) find the allocation event in local cache. Ignore event if no allocation
 *   records exists in cache.
 * 6. for every report generator:
 *    a) report generic event (without contexts)
 *    b) if context registry is not empty report event for every matching context.
 *       (if event has no contexts, it will be reported for zero context context_none).
 */
class Processor {
private:
	// the report generators
	typedef std::list<report_generator_t> generator_list_t;
	generator_list_t generators;

	// the resource registry for resource type storage
	typedef std::map<std::string, resource_registry_ptr_t> resource_map_t;
	resource_map_t resource_registry;

	// the context registry for context storage.
	typedef std::map<context_t, context_ptr_t> context_map_t;
	context_map_t context_registry;

	// the zero context, used to report allocations without contexts.
	Context context_none;

	/**
	 * Performs resource cleanup at exit.
	 */
	void cleanup();

public:

	/**
	 * Creates a new class instance.
	 */
	Processor()
	 : context_none(0, "no contexts") {
	}

	/**
	 * Destroyes the object.
	 */
	~Processor() {
		cleanup();
	}

	/**
	 * Adds a new report generator.
	 * 
	 * The report generator is managed by the processor and desroyed
	 * together with it.
	 * @param[in] generator  the report generator to add.
	 */
	void addGenerator(ReportGenerator* generator) {
		generators.push_back(report_generator_t(generator));
	}

	/**
	 * Registers a new resource type.
	 * 
	 * This method is called from parser when a resource registry record
	 * is successfully parsed.
	 * @param[in] type_id      the resource type identifier.
	 * @param[in] name         the resource type name.
	 * @param[in] ref_counted  true if the resource is reference counted.
	 */
	void registerResource(int type_id, const std::string& name, bool ref_counted);

	
	/**
	 * Registers a new allocation context.
	 * 
	 * This method is called from parser when a context registry record
	 * is successfully parsed.
	 * @param[in] value   the context value(mask).
	 * @param[in] name    the context name.
	 */
	void registerContext(context_t value, const std::string& name);

	/**
	 * Registers a new allocation event.
	 * 
	 * This method is called from parser when a function call record is
	 * successfully parsed and identified as allocation call.
	 * @param[in] index      the call index.
	 * @param[in] context    the call context.
	 * @param[in] timestamp  the call timestamp.
	 * @param[in] res_type   the allocated resource type.
	 * @param[in] res_id     the allocated resource identifier.
	 * @param[in] res_size   the allocated resource size.
	 */
	void registerAlloc(int index, context_t context, timestamp_t timestamp,
						const char* res_type, resource_id_t res_id, size_t res_size);
	
	/**
	 * Registers a new deallocation(free) event.
	 * 
	 * This method is called from parser when a function call record is
	 * successfully parsed and identified as deallocation(free) call.
	 * @param[in] index      the call index.
	 * @param[in] context    the call context.
	 * @param[in] timestamp  the call timestamp.
	 * @param[in] res_type   the allocated resource type.
	 * @param[in] res_id     the allocated resource identifier.
	 * @param res_id
	 */
	void registerFree(int index, context_t context, timestamp_t timestamp,
						const char* res_type, resource_id_t res_id);
	
	/**
	 * Reports all allocation events stored in event cache.
	 * 
	 * After the input file has been processed the event cache contains
	 * 'unfreed' allocation events. This method notifies report generators
	 * about unfreed allocation events left in cache.
	 */
	void flushEventCache();

	/**
	 * Initializes processor.
	 * 
	 * The processor initializes the registered report generators.
	 */
	void initialize();

	/**
	 * Finalizes processor.
	 * 
	 * The processor asks the registered report generators to finish
	 * processing and generate report.
	 */
	void finalize();

	/**
	 * Retrieves the number of registered generators.
	 * 
	 * @return   the number of registered generators.
	 */
	int getGeneratorCount() {
		return generators.size();
	}

};


#endif

