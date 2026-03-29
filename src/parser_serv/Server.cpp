#include <map>

#include "Server.hpp"
#include "ServerException.hpp"
#include "utils.hpp"

static bool findInLine(LineListIter it, Line line);
static void trimSemiColon(LineListIter& it);

int Server::getAllowMethods() {
	return (_allowedMethod);
}

std::string Server::getAutoIndex() {
	return (_autoIndex);
}

Server::Server(LineList servStrings)
	: _allowedMethod(GETPOSTDELETE), _autoIndex("off"), _nPort(0), _maxBodySize(-1) {
	LocParsing	 loc;
	ParsingState state = ParsingState::START;

	for (auto it = servStrings.begin(); it != servStrings.end(); it++) {
		trimSemiColon(it);
		auto list = split(*it);
		switch (state) {
			case ParsingState::START:
				state = parseStart(list, loc);
				break;
			case ParsingState::SERVER:
				state = parseServer(list, it, loc);
				break;
			case ParsingState::LOCATION:
				state = parseLocation(it, loc);
				break;
			default:
				state = ParsingState::ERROR;
				break;
		}
		if (state == ParsingState::ERROR) {
			break;
		}
	}
	checkIfConform();
}

ParsingState Server::parseStart(LineList& list, LocParsing& loc) {
	if (list[0] == "listen" && list.size() == 2) {
		setPort(list[1], loc);
	} else {
		std::cerr << "'listen' missing" << std::endl;
		throw(ServerException());
	}
	return ParsingState::SERVER;
}

ParsingState Server::parseServer(LineList& list, LineListIter it, LocParsing& loc) {
	ParsingState						state = ParsingState::SERVER;
	std::map<Line, ServerParserHandler> handlers = {
		{"allow_methods", &Server::setAllowedMethod},
		{"autoindex", &Server::setAutoIndex},
		{"client_max_body_size", &Server::setMaxBodySize},
		{"error_page", &Server::setErrorPage},
		{"index", &Server::setDefaultPage},
		{"location", &Server::parseLocation},
		{"root", &Server::setRoot},
		{"server_name", &Server::setServerName}};
	auto hit = handlers.find(list[0]);
	if (hit != handlers.end()) {
		state = (this->*(hit->second))(it, loc);
	} else {
		std::cout << "Unknown line->" << *it << "<-" << std::endl;
		throw(ServerException());
	}

	return (state);
}

ParsingState Server::parseLocation(LineListIter it, LocParsing& loc) {
	loc.open = true;
	addLocation(it, loc);
	if (!loc.open) {
		return (ParsingState::SERVER);
	}
	return (ParsingState::LOCATION);
}
void Server::checkIfConform() {
	int error_page_status[] = {400, 403, 404, 405, 413, 500, 501, 505};
	if (_root.empty() || _nPort == 0)
		throw(ServerException());
	for (auto status : error_page_status) {
		if (_errorPage.find(intToString(status)) == _errorPage.end())
			throw(ServerException());
	}
}

Server::~Server() {}

std::string& Server::getServName() {
	return (_servName);
}

int Server::getBodySize() const {
	return (_maxBodySize);
}

std::string& Server::getRoot() {
	return (_root);
}

std::string Server::getErrorPage(std::string errorNb) {
	if (_errorPage.find(errorNb) == _errorPage.end())
		throw(ServerException());
	return (_errorPage.find(errorNb)->second);
}

std::map<std::string, std::string>& Server::getAllErrorPage() {
	return (_errorPage);
}

std::vector<Location>& Server::getAllLocation() {
	return (_location);
}

int& Server::getListenPort() {
	return (_nPort);
}

std::vector<std::string>& Server::getDefaultPage() {
	return (_defaultPage);
}

Server& Server::operator=(const Server& rhs) {
	if (this != &rhs) {
		_servName = rhs._servName;
		_allowedMethod = rhs._allowedMethod;
		_autoIndex = rhs._autoIndex;
		_maxBodySize = rhs._maxBodySize;
		_root = rhs._root;
		_errorPage = rhs._errorPage;
		_location = rhs._location;
		_nPort = rhs._nPort;
		_defaultPage = rhs._defaultPage;
	}
	return (*this);
}

Server::Server(const Server& rhs) {
	*this = rhs;
}

Server::Server() {}

ParsingState Server::setAutoIndex(LineListIter it, LocParsing& loc) {
	*it = it->substr(it->find("autoindex") + 10, it->find(";") - it->find("autoindex") - 10);
	if (*it == "on" || *it == "off") {
		_autoIndex = *it;
	} else {
		std::cerr << "Autoindex needs to be on or off.\n";
		throw(ServerException());
	}
	return (ParsingState::SERVER);
}

ParsingState Server::setAllowedMethod(LineListIter it, LocParsing& loc) {
	_allowedMethod = getAllowMethodsServer(
		it->substr(it->find("allow_methods") + 14, it->find(";") - it->find("allow_methods") - 14));
	return (ParsingState::SERVER);
}

ParsingState Server::setPort(Line& line, LocParsing& loc) {
	_nPort = atoi(line.c_str());
	return (ParsingState::SERVER);
}

ParsingState Server::setRoot(LineListIter it, LocParsing& loc) {
	_root = it->substr(it->find("root") + 5, it->find(";") - it->find("root") - 5);
	return (ParsingState::SERVER);
}

ParsingState Server::setDefaultPage(LineListIter it, LocParsing& loc) {
	std::string defaultPage =
		it->substr(it->find("index") + 6, it->find(";") - it->find("index ") - 6);
	std::stringstream ss(defaultPage);
	std::string		  defaultStock;
	while (getline(ss, defaultStock, ' ')) {
		if (defaultStock != "")
			_defaultPage.push_back(defaultStock);
	}
	if (_defaultPage.size() == 0)
		throw(ServerException());
	return (ParsingState::SERVER);
}

ParsingState Server::setServerName(LineListIter it, LocParsing& loc) {
	_servName =
		it->substr(it->find("server_name") + 12, it->find(";") - it->find("server_name") - 12);
	return (ParsingState::SERVER);
}

ParsingState Server::setErrorPage(LineListIter it, LocParsing& loc) {
	std::string errorPage;
	std::string errorNb;
	if (it->length() > 11 + 3) {
		errorNb = it->substr(it->find("error_page") + 11, 3);
		if (atoi(errorNb.c_str()) < 300 || atoi(errorNb.c_str()) > 599) {
			std::cout << "Error nb page have to be lower than 599 and upper than 300" << std::endl;
			throw(ServerException());
		}
	} else
		throw(ServerException());
	if (it->find("error_page") + 15 != std::string::npos)
		errorPage =
			it->substr(it->find("error_page") + 15, it->find(";") - it->find("error_page") - 15);
	else
		throw(ServerException());
	_errorPage.insert(std::pair<std::string, std::string>(errorNb, errorPage));
	return (ParsingState::SERVER);
}

ParsingState Server::setMaxBodySize(LineListIter it, LocParsing& loc) {
	std::string maxBodySize = it->substr(it->find("client_max_body_size") + 21,
										 it->find(";") - it->find("client_max_body_size") - 21);
	if (checkIfOnlyDigits(maxBodySize) == 0)
		_maxBodySize = atoi(it->substr(it->find("max_body_size") + 14,
									   it->find(";") - it->find("max_body_size") - 14)
								.c_str());
	else {
		std::cout << "maxBodySize[maxBodySize.length() - 1] "
				  << maxBodySize[maxBodySize.length() - 1] << std::endl;
		std::cout << "client_max_body_size doesn't respect subject rules" << std::endl;
		throw(ServerException());
	}
	return (ParsingState::SERVER);
}

void Server::addLocation(LineListIter it, LocParsing& loc) {
	if (it->find("}") != std::string::npos) {
		loc.open = false;
		_location.push_back(Location(loc.lines));
		loc.lines.clear();
	} else {
		loc.lines.push_back(*it);
	}
}

static bool findInLine(LineListIter it, Line line) {
	return (it->find(line) != std::string::npos);
}

static void trimSemiColon(LineListIter& it) {
	size_t end = it->find(";");

	if (end != std::string::npos) {
		*it = it->substr(0, end);
	}
}
