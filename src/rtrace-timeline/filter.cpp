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

#include "filter.h"

SizeFilter::SizeFilter(const std::string& text) {
	size = atoi(text.c_str());
	if (size) {
		int mod = text[text.size() - 1];
		if (mod == 'K' || mod == 'k') {
			size *= 1024;
		}
		else if (mod == 'M' || mod == 'm') {
			size *= 1024 * 1024;
		}
	}
}

IndexFilter::IndexFilter(const std::string& text) {
	index = atoi(text.c_str());
	if (index) {
		int mod = text[text.size() - 1];
		if (mod == 'K' || mod == 'k') {
			index *= 1000;
		}
		else if (mod == 'M' || mod == 'm') {
			index *= 1000 * 1000;
		}
	}
}

ContextFilter::ContextFilter(const std::string& text)
	: context_mask(0) {
	std::stringstream stream(text);
	stream >> std::hex >> context_mask;
};
