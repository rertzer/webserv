#include "webserv/utils.hpp"

void replaceAll(std::string& str, std::string const& old_word, std::string const& new_word) {
	if (old_word.empty()) {
		return;
	}

	size_t start_pos = 0;

	while ((start_pos = str.find(old_word, start_pos)) != std::string::npos) {
		str.replace(start_pos, old_word.length(), new_word);
		start_pos += new_word.length();
	}
}
