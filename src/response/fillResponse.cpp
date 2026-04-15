#include "Cgi.hpp"
#include "ErrorException.hpp"
#include "Response.hpp"
#include "macroDef.hpp"
#include "utils.hpp"

void setCookie(std::string ck, Response& rep) {
	rep.getCookie().push_back(ck);
}

std::pair<std::string, std::string> extractField(size_t pos, Response& rep) {
	std::string							line;
	std::pair<std::string, std::string> field;

	if (pos != std::string::npos) {
		line = rep.getContent().substr(0, pos);
		rep.getContent().erase(0, pos + 2);
		pos = line.find(":");
		if (pos != std::string::npos) {
			field.first = line.substr(0, pos);
			field.second = line.substr(pos + 1, -1);
		}
		stringTrim(field.first);
		stringTrim(field.second);
	}
	return field;
}
