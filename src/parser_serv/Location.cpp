#include <iostream>
#include <ranges>
#include <unordered_map>

#include "ErrorException.hpp"
#include "Location.hpp"
#include "ServerException.hpp"
#include "autoindex.hpp"
#include "utils.hpp"

Location::Location(std::vector<std::string> locString)
	: autoindex(AutoIndex::NONE), redirection_status(0) {
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
		_locationLine.push_back((LineLoc)list);
	}

	if (list[0] == "root" && list.size() == 2) {
		root = list[1];
	} else if (list[0] == "autoindex" && list.size() == 2) {
		autoindex = autoIndexFromString(list[1]);
	} else if (list[0] == "index" && list.size() == 2) {
		index = list[1];
	} else if (list[0] == "return" && list.size() == 3) {
		auto stat = toInt(list[1]);
		if (stat.has_value()) {
			redirection_status = stat.value();
			redirection_path = list[2];
		} else {
			std::cerr << "return status not a valid number\n";
			throw ServerException();
		}
	} else if (list[0] == "extension" && list.size() == 2) {
		extension = list[1];
	} else if (list[0] == "cgi_path" && list.size() == 2) {
		cgi_path = list[1];
	}
}

void Location::setRoot(std::vector<std::string> list) {
	if (list.size() != 2) {
		throw ServerException();
	}
	root = list[1];
}

std::string Location::getLocationPath() const {
	return _locationPath;
}

std::vector<LineLoc>& Location::getLocationLine() {
	return _locationLine;
}
std::string Location::getIndex() const {
	return index;
}

bool Location::checkForRedirection() {
	return redirection_status;
}

std::string Location::getRoot() const {
	return root;
}

int Location::getRedirectionStatus() const {
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
