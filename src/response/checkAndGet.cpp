#include <optional>

#include "ErrorException.hpp"
#include "Response.hpp"

static std::string extractDirPath(std::string path);

void checkExec(std::string filePath) {
	if (access(filePath.c_str(), F_OK) == -1) {
		throw(ErrorException(404));
	}
	if (access(filePath.c_str(), R_OK) == -1) {
		throw(ErrorException(403));
	}
}

int CheckForRedirection(Location loc) {
	for (auto& lineloc : loc.getLocationLine()) {
		if (lineloc.getCmd() == "return")
			return (1);
	}
	return 0;
}

std::string getSpecIndex(Location loc, Response& rep) {
	for (auto& item : loc.getIndex()) {
		if (access((rep.getRoot() + item).c_str(), F_OK) != -1 &&
			access((rep.getRoot() + item).c_str(), R_OK) != -1)
			return (item);
	}
	return "";
}

Location getTheLocation(std::string path, Response& rep) {
	path = extractDirPath(path);
	auto loc = rep.getServ().findLocation(path);

	if (loc.has_value()) {
		return loc.value();
	}
	return Location();
}

static std::string extractDirPath(std::string path) {
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

int checkIfLocation(std::string path, Response& rep) {
	if (path != "/") {
		path = path.substr(0, path.rfind("."));
	}

	auto loc = rep.getServ().findLocation(path);
	return loc.has_value();
}
