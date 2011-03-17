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
#include "terminal.h"
#include "options.h"


std::ostream &operator << (std::ostream &stream, const TerminalWriter& writer) {
	return writer.write(stream);
}


std::ostream& PngTerminalHeader::write(std::ostream& stream) const {
	return stream << "set terminal png enhanced size " << Options::getInstance()->getScaleX() * 1024 / 100 <<
			"," << Options::getInstance()->getScaleY() * 768 / 100 << "\n";
	// font styling is not used, so the fontpath setting can be commented out
	//stream << "set fontpath \"/usr/share/fonts/truetype/ttf-liberation\"\n";
}


std::ostream& EpsTerminalHeader::write(std::ostream& stream) const {
	return stream << "set terminal postscript enhanced color size " << Options::getInstance()->getScaleX() * 6 / 100 <<
			"," << Options::getInstance()->getScaleY() * 4 / 100 << "\n";
}


std::ostream& WxtTerminalHeader::write(std::ostream& stream) const {
	return stream << "set terminal wxt enhanced size " << Options::getInstance()->getScaleX() * 1024 / 100 <<
			"," << Options::getInstance()->getScaleY() * 768 / 100 << "\n";
}


std::ostream& WxtTerminalFooter::write(std::ostream& stream) const {
	return stream <<"pause -1 \"The graph is displayed in other window. Hit enter when you are finished viewing it...\\n\"\n";
}
