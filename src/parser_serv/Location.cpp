#include <iostream>
#include <ranges>

#include "Location.hpp"
#include "ServerException.hpp"
#include "utils.hpp"

Location::Location(std::vector<std::string> locString) {
	auto loc = split(locString[0]);
	_locationPath = loc[1];
	for (auto& it : locString | std::views::drop(1)) {
		auto list = serverLineSplit(it);
		if (list.back() != ";") {
			std::cerr << "Location: semicolon missing " << it << std::endl;
			throw(ServerException());
		}
		list.pop_back();
		_locationLine.push_back((LineLoc)list);
	}
}

std::string Location::getLocationPath() const {
	return _locationPath;
}

std::vector<LineLoc>& Location::getLocationLine() {
	return _locationLine;
}

// Location::Location() {}

void Location::printLoc() {
	for (auto& ll : _locationLine) {
		std::cout << "Line cmd = " << ll.getCmd() << std::endl;
		for (auto& arg : ll.getArgs())
			std::cout << "Value = " << arg << std::endl;
	}
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
