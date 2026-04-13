#include <iostream>
#include <ranges>

#include "Location.hpp"
#include "ServerException.hpp"
#include "utils.hpp"

Location::Location(std::vector<std::string> locString) {
	auto loc = split(locString[0]);
	_locationPath = loc[1];
	for (auto& ls : locString | std::views::drop(1)) {
		addLine(ls);
	}
}

void Location::addLine(std::string ls) {
	auto list = serverLineSplit(ls);
	if (list.back() != ";") {
		std::cerr << "Location: semicolon missing " << ls << std::endl;
		throw(ServerException());
	}
	list.pop_back();
	if (list[0] == "root" && list.size() == 2) {
		// std::cerr << "adding root" << list[1] << std::endl;
		root = list[1];
	} else {
		_locationLine.push_back((LineLoc)list);
	}
}

std::string Location::getLocationPath() const {
	return _locationPath;
}

std::vector<LineLoc>& Location::getLocationLine() {
	return _locationLine;
}
std::vector<std::string> Location::getIndex() {
	std::vector<std::string> _stringIndex;

	for (auto& ll : _locationLine) {
		if (ll.getCmd() == "index") {
			_stringIndex = ll.getArgs();
			break;
		}
	}
	return _stringIndex;
}

bool Location::checkForRedirection() {
	for (auto& lineloc : getLocationLine()) {
		if (lineloc.getCmd() == "return")
			return (true);
	}
	return false;
}

void Location::printLoc() {
	for (auto& ll : _locationLine) {
		std::cout << "Line cmd = " << ll.getCmd() << std::endl;
		for (auto& arg : ll.getArgs())
			std::cout << "Value = " << arg << std::endl;
	}
}

std::string Location::getRoot() const {
	return root;
}
