#include <iostream>
#include <ranges>
#include <unordered_map>

#include "Location.hpp"
#include "ServerException.hpp"
#include "autoindex.hpp"
#include "macroDef.hpp"
#include "utils.hpp"

Location::Location(std::vector<std::string> locString)
	: autoindex(AutoIndex::NONE), redirection_status(HttpStatus::UNDEFINED) {
	auto loc = split(locString[0]);
	_locationPath = loc[1];
	for (auto& ls : locString | std::views::drop(1)) {
		addLine(ls);
	}
}

void Location::addLine(std::string ls) {
	std::unordered_map<std::string, LocationParsingHandler> handlers = {
		{"root", [this](const auto& t) { setRoot(t); }},
		{"autoindex", [this](const auto& t) { setAutoIndex(t); }},
		{"index", [this](const auto& t) { setIndex(t); }},
		{"return", [this](const auto& t) { setRedirection(t); }},
		{"extension", [this](const auto& t) { setExtension(t); }},
		{"cgi_path", [this](const auto& t) { setCgiPath(t); }},
		{"upload_path", [this](const auto& t) { setUploadPath(t); }},
		{"allow_methods", [this](const auto& t) { setAllowedMethods(t); }},
	};
	auto list = serverLineSplit(ls);
	if (list.back() != ";") {
		throw(ServerException());
	}
	list.pop_back();

	auto it = handlers.find(list[0]);
	if (it != handlers.end()) {
		it->second(list);
	} else {
		std::cout << "Not a valid command\n";
		throw ServerException();
	}
}

void Location::setRoot(std::vector<std::string> list) {
	if (list.size() != 2) {
		throw ServerException();
	}
	root = list[1];
}

void Location::setAutoIndex(std::vector<std::string> list) {
	if ((list.size() != 2) || (autoindex = autoIndexFromString(list[1])) == AutoIndex::NONE) {
		std::cout << "Not a valid command\n";
		throw ServerException();
	}
}

void Location::setIndex(std::vector<std::string> list) {
	if (list.size() != 2) {
		throw ServerException();
	}
	index = list[1];
}

void Location::setRedirection(std::vector<std::string> list) {
	if (list.size() != 3) {
		throw ServerException();
	}
	auto stat = toInt(list[1]);
	if (stat.has_value()) {
		switch (stat.value()) {
			case 301:
				redirection_status = HttpStatus::MOVED_PERMANENTLY;
				break;
			case 302:
				redirection_status = HttpStatus::FOUND;
				break;
			default:
				break;
		}
		redirection_path = list[2];
	}
	if (redirection_status == HttpStatus::UNDEFINED) {
		std::cerr << "return status not a valid number\n";
		throw ServerException();
	}
}

void Location::setExtension(std::vector<std::string> list) {
	if (list.size() != 2) {
		throw ServerException();
	}
	extension = list[1];
}

void Location::setCgiPath(std::vector<std::string> list) {
	if (list.size() != 2) {
		throw ServerException();
	}
	cgi_path = list[1];
}

void Location::setUploadPath(std::vector<std::string> list) {
	if (list.size() != 2) {
		throw ServerException();
	}
	upload_path = list[1];
}

void Location::setAllowedMethods(std::vector<std::string> list) {
	allowed_method = getAllowMethodsServer(list);
}

std::string Location::getLocationPath() const {
	return _locationPath;
}

std::string Location::getIndex() const {
	return index;
}

bool Location::checkForRedirection() {
	return (redirection_status != HttpStatus::UNDEFINED);
}

std::string Location::getRoot() const {
	return root;
}

HttpStatus Location::getRedirectionStatus() const {
	return redirection_status;
}

std::string Location::getRedirectionPath() const {
	return redirection_path;
}

AutoIndex Location::getAutoindex() const {
	return autoindex;
}

std::string Location::getExtension() const {
	return extension;
}
std::string Location::getCgiPath() const {
	return cgi_path;
}
std::string Location::getUploadPath() const {
	return upload_path;
}

BitSet Location::getAllowedMethods() const {
	return allowed_method;
}

bool Location::isAllowed(HttpMethod method) const {
	return allowed_method.isSet(method);
}
bool Location::isRooted() const {
	return !root.empty();
}
