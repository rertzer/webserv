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

BitSet checkAllowMethod(Location loc) {
	BitSet methods;

	for (auto& line : loc.getLocationLine()) {
		if (line.getCmd() == "allow_methods") {
			if (line.checkArgs("GET") == 1)
				methods.addFlag(GET);
			if (line.checkArgs("POST") == 1)
				methods.addFlag(POST);
			if (line.checkArgs("DELETE") == 1)
				methods.addFlag(DELETE);
		}
	}

	return methods;
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

int checkAutoIndex(Location loc) {
	for (auto& ll : loc.getLocationLine()) {
		if (ll.getCmd() == "autoindex") {
			if (ll.getArgs()[0] == "on")
				return 1;
			else
				return 0;
		}
	}
	return -1;
}

int checkForRedirection(Location& loc) {
	for (auto& ll : loc.getLocationLine()) {
		if (ll.getCmd() == "return") {
			if (ll.getArgs().size() >= 2)
				return 1;
		}
	}
	return 0;
}

std::pair<std::string, std::string> RedirectTo(Location& loc) {
	std::pair<std::string, std::string> ret;
	for (auto& ll : loc.getLocationLine()) {
		if (ll.getCmd() == "return") {
			ret.first = ll.getArgs()[0];
			ret.second = ll.getArgs()[1];
			return (ret);
		}
	}
	return ret;
}

int isThereAspecRoot(Location& loc) {
	for (auto& ll : loc.getLocationLine()) {
		if (ll.getCmd() == "root")
			return 1;
	}
	return 0;
}

std::string getArgsLoc(Location& loc, std::string toFind) {
	for (auto& ll : loc.getLocationLine()) {
		if (ll.getCmd() == toFind)
			return ll.getArgs()[0];
	}
	return "";
}

void printServ(Server& serv) {
	std::cout << "Server name : " << serv.getServName() << std::endl;
	std::cout << "Server root : " << serv.getRoot() << std::endl;
	std::cout << "Server Auto index : " << serv.getAutoIndex() << std::endl;
	std::cout << "Server allowed method : " << static_cast<int>(serv.getAllowMethods().getFlags())
			  << std::endl;
	std::cout << "Listening on port : " << serv.getListenPort() << std::endl;
	for (std::map<std::string, std::string>::iterator it = serv.getAllErrorPage().begin();
		 it != serv.getAllErrorPage().end(); it++)
		std::cout << it->first << " : " << it->second << std::endl;

	std::cout << "--------Location-------------" << std::endl;

	std::vector<Location> loc = serv.getAllLocation();
	for (std::vector<Location>::iterator it = loc.begin(); it != loc.end(); it++) {
		std::cout << "Location path : " << it->getLocationPath() << std::endl;
		std::cout << "Location line : " << std::endl;
		it->printLoc();
	}
	std::cout << "----------------------------------------" << std::endl;
}

std::pair<std::string, std::string> getExtension(Location loc) {
	std::pair<std::string, std::string> ret;
	for (auto& ll : loc.getLocationLine()) {
		if (ll.getCmd() == "extension")
			ret.first = ll.getArgs()[0];
		if (ll.getCmd() == "cgi_path")
			ret.second = ll.getArgs()[0];
	}
	return ret;
}

int checkIfOnlyDigits(std::string str) {
	for (auto l : str) {
		if (!isdigit(l))
			return 1;
	}
	return 0;
}

std::string getUploadPath(Location loc) {
	std::string ret;
	for (auto& ll : loc.getLocationLine()) {
		if (ll.getCmd() == "upload_path")
			ret = ll.getArgs()[0];
	}
	return ret;
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
