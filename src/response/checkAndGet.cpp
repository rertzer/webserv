#include <unistd.h>
#include <string>

#include "ErrorException.hpp"

void checkExec(std::string filePath) {
	if (access(filePath.c_str(), F_OK) == -1) {
		throw(ErrorException(404));
	}
	if (access(filePath.c_str(), R_OK) == -1) {
		throw(ErrorException(403));
	}
}

std::string extractDirPath(std::string path) {
	if (path != "/") {
		auto pos = path.rfind(".");
		if (pos != std::string::npos) {
			path = path.substr(0, path.rfind("."));
		}
		pos = path.rfind("/");
		if (pos != std::string::npos) {
			path = path.substr(0, path.rfind("/"));
		}
	}
	return path;
}
