
#include "timeline.h"

#include "parser.h"
#include "formatter.h"
#include "processor.h"

bool Parser::parseCallRecord(const char* buffer)
{
	char res_type_name[512];
	unsigned int index;
	context_t context = 0;
	int hours, minutes, seconds, mseconds;
	timestamp_t timestamp = 0;
	resource_id_t res_id;
	size_t res_size;
	char name[512], delim;
	const char*ptr = buffer;
	char* res_type;
	/* parse index field <index>. */
	if (sscanf(ptr, "%d%c", &index, &delim) != 2 || delim != '.') return false;
	/* move cursor beyond index field */
	ptr = strchr(ptr, ' ');
	if (!ptr) return false;
	ptr++;

	/* parse optional context mask */
	if (sscanf(ptr, "@%x", &context) == 1) {
		/* context mask was parsed successfully. Move cursor to next field */
		ptr = strchr(ptr, '@');
		ptr = strchr(ptr, ' ');
		if (!ptr) return false;
		ptr++;
	}
	/* parse optional timestamp */
	if (sscanf(ptr, "[%d:%d:%d.%d]", &hours, &minutes, &seconds, &mseconds) == 4) {
		/* timestamp was parsed successfully. Move cursor beyond timestamp */
		timestamp = hours * 60 * 60 * 1000 + minutes * 60 * 1000 + seconds * 1000 + mseconds;
		ptr = strchr(ptr, ' ');
		if (!ptr) return false;
		ptr++;
	}
	if (sscanf(ptr, "%[^(<]", name) != 1) {
		return false;
	}
	ptr += strlen(name);
	if (*ptr == '<') {
		if (sscanf(ptr, "<%[^>]>", res_type_name) == 0) {
			return false;
		}
		ptr = strchr(ptr, '(');
		if (!ptr) return false;
		res_type = res_type_name;
	}
	else {
		res_type = NULL;
	}
	if (sscanf(ptr, "(%u) = 0x%lx", &res_size, &res_id) == 2) {
		processor->registerAlloc(index, context, timestamp, res_type, res_id, res_size);
		return true;
	}
	else if (sscanf(ptr, "(0x%lx)", &res_id) == 1) {
		processor->registerFree(index, context, timestamp, res_type, res_id);
		return true;
	}
	return false;
}


bool Parser::parseContextRecord(const char* buffer)
{
	char name[512];
	unsigned int id;
	if (sscanf(buffer, "@ %x : %[^\n]", &id, name) != 2) return false;
	processor->registerContext(id, name);
	return false;
}

bool Parser::parseResourceRecord(const char* buffer)
{
	char type[512], desc[512];
	unsigned int id;
	if (sscanf(buffer, "<%x> : %[^ ] (%[^)])", &id, type, desc) != 3) return false;
	processor->registerResource(ffs(id), type, false);
	return true;
}

void Parser::parseFile(const std::string& filename, Processor* processor)
{
	char buffer[4096];
	std::ifstream in(filename.c_str());

	if (in.fail()) {
		throw std::ios_base::failure(Formatter() << "Failed to open file: " << filename);
	}
	this->processor = processor;

	while (true) {
		in.getline(buffer, sizeof(buffer));
		if (in.eof()) break;
		if (*buffer == '\0' || *buffer == '\t') continue;
		if (parseCallRecord(buffer)) continue;
		if (parseResourceRecord(buffer)) continue;
		if (parseContextRecord(buffer)) continue;
	}

}
