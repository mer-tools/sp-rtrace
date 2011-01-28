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


#include "timeline.h"

#include "parser.h"
#include "common/formatter.h"
#include "processor.h"

extern "C" {
#include "library/sp_rtrace_defs.h"
#include "library/sp_rtrace_parser.h"
}

void Parser::parseFile(const std::string& filename, Processor* processor)
{
	char buffer[4096];
	std::ifstream in(filename.c_str());

	if (in.fail()) {
		throw std::ios_base::failure(Formatter() << "Failed to open file: " << filename);
	}
	this->processor = processor;

	sp_rtrace_parser_set_mask(SP_RTRACE_RECORD_CALL | SP_RTRACE_RECORD_RESOURCE | SP_RTRACE_RECORD_CONTEXT);

	while (true) {
		in.getline(buffer, sizeof(buffer));
		if (in.eof()) break;
		sp_rtrace_record_t rec;
		int rec_type = sp_rtrace_parser_parse_record(buffer, &rec);
		switch (rec_type) {
			case SP_RTRACE_RECORD_CALL:
				if (rec.call.type == SP_RTRACE_FTYPE_ALLOC) {
					processor->registerAlloc(rec.call.index, rec.call.context, rec.call.timestamp, (char*)rec.call.res_type, rec.call.res_id, rec.call.res_size);
				}
				else  {
					processor->registerFree(rec.call.index, rec.call.context, rec.call.timestamp, (char*)rec.call.res_type, rec.call.res_id);
				}
				break;

			case SP_RTRACE_RECORD_RESOURCE:
				processor->registerResource(ffs(rec.resource.id), rec.resource.type, false);
				break;

			case SP_RTRACE_RECORD_CONTEXT:
				processor->registerContext(rec.context.id, rec.context.name);
				break;

			case SP_RTRACE_RECORD_NONE:
				continue;
		}
		sp_rtrace_parser_free_record(rec_type, &rec);
	}

}
