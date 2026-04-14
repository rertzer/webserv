#include <charconv>
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

std::string intToString(int n) {
	std::stringstream ss;
	ss << n;
	return ss.str();
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
		if (req.getField("Host") == it->getServName() + ":" + intToString(req.getPort())) {
			if (listeningPort == it->getListenPort())
				return *it;
		}
		it++;
	};
	return findTheDefaultServ(serv, listeningPort);
}

Response createErrorPage(int codeErr, Server serv) {
	ContentMap	contentMap;
	std::string status = Status::getMsg(codeErr);
	std::string contentType = contentMap.getContentValue(
		serv.getErrorPage(intToString(codeErr))
			.substr(serv.getErrorPage(intToString(codeErr)).rfind(".") + 1,
					serv.getErrorPage(intToString(codeErr)).length()));
	std::string content;
	try {
		content = readSpecFile(serv.getRoot() + serv.getErrorPage(intToString(codeErr)));
	} catch (const std::exception& e) {
		content = "<html><body>File deleted</body></html>";
	}
	std::string contentLength = intToString(content.length());
	std::string connectionClose = "close";
	Response	errResp(status, contentType, contentLength, connectionClose, content);
	return errResp;
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

std::pair<std::string, std::string> getExtension(Location loc) {
	std::pair<std::string, std::string> ret;
	auto								extension = loc.getExtension();
	auto								path = loc.getCgiPath();
	if (extension.empty() || path.empty()) {
		return ret;
	}

	ret.first = extension;
	ret.second = path;
	return ret;
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

std::optional<int> toInt(const std::string& str) {
	int value;
	auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);

	if (ec == std::errc() && ptr == str.data() + str.size())
		return value;

	return std::nullopt;
}
