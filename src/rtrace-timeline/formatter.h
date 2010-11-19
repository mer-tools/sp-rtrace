#ifndef _FORMATTER_H_
#define _FORMATTER_H_

#include "timeline.h"

/**
 * Helper class for easier string formatting.
 *
 * Usage example (of course /proc/self/maps could be used, but that's not the point):
 * std::ifstream maps_file(Formatter() << "/proc/" << getpid() << "/maps");
 */
class Formatter {
private:
    std::stringstream stream;

public:
    operator std::string() const { return stream.str(); }

    template<class T>
    Formatter& operator<<(T const& value) { stream << value; return *this; }
};

#endif
