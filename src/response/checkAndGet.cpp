#include "ErrorException.hpp"
#include "Response.hpp"

static std::string extractDirPath(std::string path);
static Location*   findLocationByPath(std::vector<Location>& all_loc, std::string path);

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
	std::vector<Location> all_loc = rep.getServ().getAllLocation();
	Location*			  loc = nullptr;
	path = extractDirPath(path);
	while (path.find("/") != std::string::npos) {
		loc = findLocationByPath(all_loc, path);
		if (loc || path.rfind("/") == 0) {
			break;
		}
		path = path.substr(0, path.rfind("/"));
	}
	if (loc)
		return *loc;
	return Location();
}

static std::string extractDirPath(std::string path) {
	if (path != "/") {
		path = path.substr(0, path.rfind("."));
		path = path.substr(0, path.rfind("/"));
	}
	return path;
}

static Location* findLocationByPath(std::vector<Location>& all_loc, std::string path) {
	for (auto& loc : all_loc) {
		if (loc.getLocationPath() == path) {
			return &loc;
		}
	}
	return nullptr;
}

int checkIfLocation(std::string path, Response& rep) {
	std::vector<Location> loc = rep.getServ().getAllLocation();
	bool				  enter = false;
	std::cout << "arrive here" << std::endl;
	if (path != "/") {
		path = path.substr(0, path.rfind("."));
	}
	while (path.find("/") != std::string::npos) {
		for (std::vector<Location>::iterator it = loc.begin(); it != loc.end(); it++) {
			if (it->getLocationPath() == path) {
				return (0);
			}
		}
		if (path.rfind("/") == 0) {
			if (enter == false) {
				path = "/";
				enter = true;
			} else
				return (-1);
		} else {
			path = path.substr(0, path.rfind("/"));
		}
		std::cout << "boucled here" << std::endl;
	}
	return -1;
}
