#include "webserv/utils.hpp"

static size_t getNextWord(LineList& splited, Line const& line, std::string sep, size_t start);

LineList extract(Line const& line, std::string sep) {
	LineList splited;
	size_t	 start = 0;
	if (line == sep) {
		splited.push_back(line);
		return splited;
	}
	while ((start = line.find_first_not_of(sep, start)) != std::string::npos) {
		start = getNextWord(splited, line, sep, start);
	}
	return splited;
}

static size_t getNextWord(LineList& splited, Line const& line, std::string sep, size_t start) {
	size_t end = line.find_first_of(sep, start + 1);
	auto   word = line.substr(start, end - start);
	splited.push_back(word);
	if (end != std::string::npos) {
		splited.push_back(sep);
	}
	return end;
}
