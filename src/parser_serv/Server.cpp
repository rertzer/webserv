#include <iostream>
#include <map>
#include <ranges>

#include "Server.hpp"
#include "ServerException.hpp"
#include "utils.hpp"

static void trimAfterSemiColon(LineListIter& it);

BitSet Server::getAllowMethods() const {
	return allowed_method;
}

std::string Server::getAutoIndex() {
	return _autoIndex;
}

Server::Server(LineList servStrings)
	: allowed_method(GET | POST | DELETE), _autoIndex("off"), _nPort(0), _maxBodySize(-1) {
	LocParsing	 loc;
	ParsingState state = ParsingState::START;

	for (auto it = servStrings.begin(); it != servStrings.end(); it++) {
		trimAfterSemiColon(it);
		auto list = serverLineSplit(*it);
		switch (state) {
			case ParsingState::START:
				state = parseStart(list, loc);
				break;
			case ParsingState::SERVER:
				state = parseServer(list, loc);
				break;
			case ParsingState::LOCATION:
				state = parseLocation(list, loc);
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
	loadHtmlCode();
}

ParsingState Server::parseStart(LineList& list, LocParsing& loc) {
	if (list.size() == 3 && list[0] == "listen" && list[2] == ";") {
		setPort(list[1], loc);
	} else {
		std::cerr << "'listen' missing" << std::endl;
		throw(ServerException());
	}
	return ParsingState::SERVER;
}

ParsingState Server::parseServer(LineList& list, LocParsing& loc) {
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
	if (hit != handlers.end() && (list[0] == "location" || list.back() == ";")) {
		state = (this->*(hit->second))(list, loc);
	} else {
		std::cerr << "Unknown line: " << list[0] << std::endl;
		throw(ServerException());
	}

	return state;
}

ParsingState Server::parseLocation(LineList& list, LocParsing& loc) {
	loc.open = true;
	addLocation(list, loc);
	if (!loc.open) {
		return ParsingState::SERVER;
	}
	return ParsingState::LOCATION;
}
void Server::checkIfConform() {
	int error_page_status[] = {400, 403, 404, 405, 413, 500, 501, 505};
	if (_root.empty() || _nPort == 0) {
		throw(ServerException());
	}
	for (auto status : error_page_status) {
		if (_errorPage.find(intToString(status)) == _errorPage.end())
			throw(ServerException());
	}
}

Server::~Server() {}

std::string& Server::getServName() {
	return _servName;
}

int Server::getBodySize() const {
	return _maxBodySize;
}

std::string& Server::getRoot() {
	return _root;
}

std::string Server::getErrorPage(std::string errorNb) {
	if (_errorPage.find(errorNb) == _errorPage.end()) {
		throw(ServerException());
	}
	return _errorPage.find(errorNb)->second;
}

std::map<std::string, std::string>& Server::getAllErrorPage() {
	return _errorPage;
}

std::vector<Location>& Server::getAllLocation() {
	return _location;
}

int& Server::getListenPort() {
	return _nPort;
}

std::vector<std::string>& Server::getDefaultPage() {
	return _defaultPage;
}

Server& Server::operator=(const Server& rhs) {
	if (this != &rhs) {
		_servName = rhs._servName;
		allowed_method = rhs.allowed_method;
		_autoIndex = rhs._autoIndex;
		_maxBodySize = rhs._maxBodySize;
		_root = rhs._root;
		_errorPage = rhs._errorPage;
		_location = rhs._location;
		_nPort = rhs._nPort;
		_defaultPage = rhs._defaultPage;
		html_code = rhs.html_code;
	}
	return *this;
}

Server::Server(const Server& rhs) {
	*this = rhs;
}

Server::Server() {}

ParsingState Server::setAutoIndex(LineList& list, LocParsing& loc) {
	(void)loc;
	if (list.size() == 3 && (list[1] == "on" || list[1] == "off")) {
		_autoIndex = list[1];
	} else {
		std::cerr << "Autoindex needs to be on or off.\n";
		throw(ServerException());
	}
	return ParsingState::SERVER;
}

ParsingState Server::setAllowedMethod(LineList& list, LocParsing& loc) {
	(void)loc;
	list.pop_back();
	allowed_method = getAllowMethodsServer(list);
	return ParsingState::SERVER;
}

ParsingState Server::setPort(Line& line, LocParsing& loc) {
	(void)loc;
	auto p = toInt(line);
	if (p.has_value()) {
		_nPort = p.value();
	} else {
		std::cerr << "Invalid port value\n";
		throw(ServerException());
	}
	return ParsingState::SERVER;
}

ParsingState Server::setRoot(LineList& list, LocParsing& loc) {
	(void)loc;
	if (list.size() != 3) {
		throw(ServerException());
	}
	_root = list[1];
	return ParsingState::SERVER;
}

ParsingState Server::setDefaultPage(LineList& list, LocParsing& loc) {
	(void)loc;
	if (list.size() < 3) {
		throw(ServerException());
	}
	for (auto& it : list | std::views::drop(1) | std::views::take(list.size() - 2)) {
		_defaultPage.push_back(it);
	}
	if (_defaultPage.size() == 0) {
		throw(ServerException());
	}
	return ParsingState::SERVER;
}

ParsingState Server::setServerName(LineList& list, LocParsing& loc) {
	(void)loc;
	if (list.size() != 3) {
		throw(ServerException());
	}
	_servName = list[1];
	return ParsingState::SERVER;
}

ParsingState Server::setErrorPage(LineList& list, LocParsing& loc) {
	(void)loc;

	if (list.size() < 3) {
	}
	std::string errorNb = list[1];
	std::string errorPage = list[2];
	auto		number = toInt(errorNb);
	if (!number.has_value() || number.value() < 300 || number.value() > 599) {
		std::cerr << "Error page number out of range" << std::endl;
		throw(ServerException());
	}
	_errorPage.insert(std::pair<std::string, std::string>(errorNb, errorPage));
	return ParsingState::SERVER;
}

ParsingState Server::setMaxBodySize(LineList& list, LocParsing& loc) {
	(void)loc;
	if (list.size() != 3) {
		throw(ServerException());
	}
	auto mbs = toInt(list[1]);
	if (mbs.has_value()) {
		_maxBodySize = mbs.value();
	} else {
		std::cerr << "Invalid client_max_body_size\n";
		throw(ServerException());
	}
	return ParsingState::SERVER;
}

void Server::addLocation(LineList& list, LocParsing& loc) {
	if (list.size() == 1 && list[0] == "}") {
		loc.open = false;
		_location.push_back(Location(loc.lines));
		loc.lines.clear();
	} else {
		loc.lines.push_back(join(list));
	}
}

void Server::loadHtmlCode() {
	html_code[HtmlCode::AUTOINDEX_HEADER] = getFileContent("data/index_header.html");
	html_code[HtmlCode::AUTOINDEX_CONTENT] = getFileContent("data/index_content.html");
	html_code[HtmlCode::AUTOINDEX_FOOTER] = getFileContent("data/index_footer.html");
}

std::string Server::getHtmlCode(HtmlCode kind) {
	return html_code[kind];
}

bool Server::isAllowed(HttpMethod method) const {
	return allowed_method.isSet(method);
}

static void trimAfterSemiColon(LineListIter& it) {
	size_t end = it->find(";");

	if (end != std::string::npos) {
		*it = it->substr(0, end + 1);
	}
}

LocationRefOpt Server::findLocation(std::string path) {
	LocationRefOpt loc;
	while (path.find("/") != std::string::npos) {
		loc = findLocationByPath(path);
		if (loc.has_value() || path.rfind("/") == 0) {
			return loc;
		}
		path = path.substr(0, path.rfind("/"));
	}
	return std::nullopt;
}

LocationRefOpt Server::findLocationByPath(std::string path) {
	for (auto& loc : _location) {
		if (loc.getLocationPath() == path) {
			return loc;
		}
	}
	return std::nullopt;
}
