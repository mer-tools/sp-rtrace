
#include "timeline.h"

#include "parser.h"
#include "formatter.h"
#include "processor.h"

extern "C" {
#include "common/sp_rtrace_defs.h"
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
