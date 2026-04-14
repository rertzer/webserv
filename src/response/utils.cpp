#include <charconv>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "ErrorException.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include "ServerException.hpp"
#include "Status.hpp"
#include "macroDef.hpp"

std::string readSpecFile(std::string file) {
	std::ifstream fileOp;
	fileOp.open(file.c_str());
	if (access(file.c_str(), F_OK) == -1) {
		throw(ErrorException(404));
	}
	if (access(file.c_str(), R_OK) == -1) {
		throw(ErrorException(403));
	}
	if (fileOp.is_open()) {
		std::stringstream fileStr;
		fileStr << fileOp.rdbuf();
		fileOp.close();
		return fileStr.str();
	} else {
		fileOp.close();
		throw(ErrorException(404));
	}
}
int isDir(std::string fileName) {
	struct stat path;

	memset(&path, 0, sizeof(path));
	stat(fileName.c_str(), &path);

	return S_ISREG(path.st_mode);
}
bool isReadable(const std::string& path) {
	namespace fs = std::filesystem;

	auto p = fs::status(path).permissions();

	return (p & fs::perms::owner_read) != fs::perms::none ||
		   (p & fs::perms::group_read) != fs::perms::none ||
		   (p & fs::perms::others_read) != fs::perms::none;
}

Server& findTheDefaultServ(std::vector<Server>& serv, int listeningPort) {
	std::vector<Server>::iterator it = serv.begin();

	while (it != serv.end()) {
		if (it->getListenPort() == listeningPort) {
			return *it;
		}
		it++;
	}
	throw(ServerException());
	return *(serv.begin());
}

Server& findTheServ(Request& req, std::vector<Server>& serv, int listeningPort) {
	std::vector<Server>::iterator it = serv.begin();
	while (it != serv.end()) {
		if (req.getField("Host") == it->getServName() + ":" + std::to_string(req.getPort())) {
			if (listeningPort == it->getListenPort())
				return *it;
		}
		it++;
	};
	return findTheDefaultServ(serv, listeningPort);
}

BitSet getAllowMethodsServer(LineList const& list) {
	BitSet methods;
	for (auto it = std::next(list.begin()); it != list.end(); ++it) {
		HttpMethod method = stringToMethod(*it);
		if (method != NONE) {
			methods.addFlag(method);
		} else {
			throw(ServerException());
		}
	}

	return methods;
}

int isThereAspecRoot(Location& loc) {
	if (loc.getRoot().empty()) {
		return 0;
	}
	return 1;
}

int checkIfOnlyDigits(std::string str) {
	for (auto l : str) {
		if (!isdigit(l))
			return 1;
	}
	return 0;
}

std::string join(LineList const& list) {
	if (list.empty())
		return "";

	std::ostringstream oss;
	auto			   it = list.begin();

	oss << *it++;  // first element

	for (; it != list.end(); ++it)
		oss << ' ' << *it;

	return oss.str();
}

std::string fileExtension(std::string const& filename) {
	std::string extension;
	auto		lastdot = filename.rfind(".");
	if (lastdot != std::string::npos) {
		extension = filename.substr(lastdot + 1);
	}
	return extension;
}

std::optional<int> toInt(const std::string& str) {
	int value;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);

	if (ec == std::errc() && ptr == str.data() + str.size())
		return value;

	return std::nullopt;
}
