#ifndef _FILTER_H_
#define _FILTER_H_

#include "event.h"
#include "timestamp.h"

/**
 * The base class for all filters.
 */
class Filter {
public:

	/**
	 * Validates the event.
	 *
	 * @param[in] event  the event to validate.
	 * @return           true if the event passes the filter rules,
	 *                   false otherwise.
	 */
	virtual bool validate(const Event* event) = 0;

};
typedef std::tr1::shared_ptr<Filter> filter_ptr_t;


/**
 * The base class for size filters.
 */
class SizeFilter : public Filter {
protected:
	// the validation value
	size_t size;

	/**
	 * Creates a new class instance.
	 *
	 * The size filter uses the following format - <size>[<modifier>]
	 * Where:
	 *   <size>     - the size base value
	 *   <modifier> - the size modifier - m|M|k|K
	 *     k|K      - the size base value is specified in kilobytes
	 *     m|M      - the size base value is specified in megabytes
	 * @param[in] text  the filter value textual interpretation.
	 */
	SizeFilter(const std::string& text);
};

/**
 * The minimal size filter passes allocation events with resource
 * size greater or equal. It also passes all deallocation events.
 */
class MinSizeFilter : public SizeFilter {
public:
	/**
	 * @copydoc SizeFilter::SizeFilter
	 */
	MinSizeFilter(const std::string& text)
		: SizeFilter(text) {
	}

	/**
	 * @copydoc Filter::validate
	 */
	bool validate(const Event* event) {
		return !event->res_size || event->res_size >= size;
	}
};

/**
 * The maximal size filter passes allocation events with resource
 * size less or equal. It also passes all deallocation events.
 */
class MaxSizeFilter : public SizeFilter {
public:
	/**
	 * @copydoc SizeFilter::SizeFilter
	 */
	MaxSizeFilter(const std::string& text)
		: SizeFilter(text) {
	}

	/**
	 * @copydoc Filter::validate
	 */
	bool validate(const Event* event) {
		return !event->res_size || event->res_size <= size;
	}
};


/**
 * The base class for index filters.
 */
class IndexFilter : public Filter {
protected:
	// the validation value
	int index;

	/**
	 * Creates a new class instance.
	 *
	 * The size filters uses the following format - <size>[<modifier>]
	 * Where:
	 *   <size>     - the size base value
	 *   <modifier> - the size modifier - m|M|k|K
	 *     k|K      - the size base value is specified in thousands.
	 *     m|M      - the size base value is specified in millions.
	 * @param[in] text  the filter value textual interpretation.
	 */
	IndexFilter(const std::string& text);
};

/**
 * The minimal index filter passes all events with index greater or
 * equal.
 */
class MinIndexFilter : public IndexFilter {
public:
	/**
	 * @copydoc IndexFilter::IndexFilter
	 */
	MinIndexFilter(const std::string& text)
		: IndexFilter(text) {
	}

	/**
	 * @copydoc Filter::validate
	 */
	bool validate(const Event* event) {
		return event->index >= index;
	}
};

/**
 * The maximal index filter passes all events with index less or
 * equal.
 */
class MaxIndexFilter : public IndexFilter {
public:
	/**
	 * @copydoc IndexFilter::IndexFilter
	 */
	MaxIndexFilter(const std::string& text)
		: IndexFilter(text) {
	}

	/**
	 * @copydoc Filter::validate
	 */
	bool validate(const Event* event) {
		return event->index <= index;
	}
};

/**
 * The base clas for time filters.
 */
class TimeFilter : public Filter {
protected:
	// the validation value
	timestamp_t timestamp;

	/**
	 * Creates a new class instance.
	 *
	 * The time filters uses the following format -
	 * [+][<hours>:][<minutes>:]<seconds>[.<milliseconds>]
	 * The '+' is ignored, while the rest of value is parsed
	 * according to the Timestamp parsing rules.
	 * @param text  the filter value textual interpretation.
	 * @return
	 */
	TimeFilter(const std::string& text) {
		timestamp = Timestamp::fromString(text);
	}
};

/**
 * The minimal time filter passes all events with timestamp
 * greater or equal.
 */
class MinTimeFilter : public TimeFilter {
public:

	/**
	 * @copydoc TimeFilter::TimeFilter
	 */
	MinTimeFilter(const std::string& text)
		: TimeFilter(text) {
	}

	/**
	 * @copydoc Filter::validate
	 */
	bool validate(const Event* event) {
		return event->timestamp >= timestamp;
	}
};

/**
 * The minimal time filter passes all events with timestamp
 * less or equal.
 */
class MaxTimeFilter : public TimeFilter {
public:

	/**
	 * @copydoc TimeFilter::TimeFilter
	 */
	MaxTimeFilter(const std::string& text)
		: TimeFilter(text) {
	}

	/**
	 * @copydoc Filter::validate
	 */
	bool validate(const Event* event) {
		return event->timestamp <= timestamp;
	}
};

/**
 * The minimal time offset filter passes all events that occurred
 * after the specified time offset since the first event.
 */
class MinTimeOffsetFilter : public TimeFilter {
private:
	timestamp_t start_timestamp;
public:

	/**
	 * @copydoc TimeFilter::TimeFilter
	 */
	MinTimeOffsetFilter(const std::string& text)
		: TimeFilter(text), start_timestamp(0) {
	}

	/**
	 * @copydoc Filter::validate
	 */
	bool validate(const Event* event) {
		if (start_timestamp == 0) start_timestamp = event->timestamp;
		return event->timestamp >= timestamp + start_timestamp;
	}
};

/**
 * The maximal time offset filter passes all events that occurred
 * before the specified time offset since the first matched event.
 */
class MaxTimeOffsetFilter : public TimeFilter {
private:
	timestamp_t start_timestamp;
public:

	/**
	 * @copydoc TimeFilter::TimeFilter
	 */
	MaxTimeOffsetFilter(const std::string& text)
		: TimeFilter(text), start_timestamp(0) {
	}

	/**
	 * @copydoc Filter::validate
	 */
	bool validate(const Event* event) {
		if (start_timestamp == 0) start_timestamp = event->timestamp;
		return event->timestamp <= timestamp + start_timestamp;
	}
};

/**
 * The context filter passes all events with context mask
 * matching the filter context or events without context
 * mask if the filter context mask is zero.
 */
class ContextFilter : public Filter {
protected:
	context_t context_mask;

public:

	/**
	 * Creates a new class instance.
	 *
	 * The context mask is specified as hexadecimal value.
	 * @param text  the filter value textual interpretation.
	 * @return
	 */
	ContextFilter(const std::string& text);

	/**
	 * @copydoc Filter::validate
	 */
	bool validate(const Event* event) {
		return (context_mask == 0 && event->context == 0) || (context_mask & event->context);
	}

};

#endif

