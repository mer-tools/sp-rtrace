/*
 * This file is part of sp-rtrace package.
 *
 * Copyright (C) 2010 by Nokia Corporation
 *
 * Contact: Eero Tamminen <eero.tamminen@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02r10-1301 USA
 */

#ifndef _FORMATTER_H_
#define _FORMATTER_H_

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
