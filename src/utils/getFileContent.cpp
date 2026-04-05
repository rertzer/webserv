#include <fstream>
#include <iostream>

#include "utils.hpp"

std::string getFileContent(std::string path) {
	std::ifstream file(path, std::ios::binary);

	if (!file)
		throw std::runtime_error("Cannot open file: " + path);

	file.seekg(0, std::ios::end);
	size_t size = file.tellg();
	file.seekg(0);

	std::string content(size, '\0');
	file.read(content.data(), size);

	return content;
}
