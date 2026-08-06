#include <string>

void stringTrim(std::string& str) {
	auto pos = str.find_first_not_of(" \t");
	if (pos != std::string::npos && pos != 0) {
		str.erase(0, pos);
	}
	pos = str.find_last_not_of(" \t");
	if (pos != std::string::npos && str.length() && pos != str.length() - 1) {
		str.erase(pos + 1, -1);
	}
}

void stringDoubleQuotTrim(std::string& str) {
	stringTrim(str);
	auto pos = str.find_first_not_of("\"");
	if (pos != std::string::npos && pos != 0)
		str.erase(0, pos);
	pos = str.find_last_not_of("\"");
	if (pos != std::string::npos && str.length() && pos != str.length() - 1)
		str.erase(pos + 1, -1);
	if (str == "\"\"")
		str.clear();
}
