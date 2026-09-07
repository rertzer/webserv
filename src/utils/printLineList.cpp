#include <iostream>

#include "webserv/utils.hpp"

void printLineList(LineList const& list) {
	for (auto& line : list) {
		std::cerr << line << "\n";
	}
	std::cerr << std::endl;
}
