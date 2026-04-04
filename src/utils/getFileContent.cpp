#include <fstream>
#include <iostream>
#include <sstream>

#include "utils.hpp"

std::string getFileContent(std::string path) {
	std::ifstream file;
	try {
		file.open(path, std::fstream::in);
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	return buffer.str();
}
