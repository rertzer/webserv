#include <iostream>
#include <ranges>

#include "Location.hpp"
#include "utils.hpp"

Location::Location(std::vector<std::string> locString) {
	auto loc = split(locString[0]);
	_locationPath = loc[1];
	for (auto& it : locString | std::views::drop(1)) {
		auto list = split(it);
		_locationLine.push_back((LineLoc)list);
	}
}

std::string Location::getLocationPath() const {
	return _locationPath;
}

std::vector<LineLoc>& Location::getLocationLine() {
	return _locationLine;
}

Location::Location() {}

void Location::printLoc() {
	for (std::vector<LineLoc>::iterator it = _locationLine.begin(); it != _locationLine.end();
		 it++) {
		std::cout << "Line cmd = " << it->getCmd() << std::endl;
		for (std::vector<std::string>::iterator it2 = it->getArgs().begin();
			 it2 != it->getArgs().end(); it2++)
			std::cout << "Value = " << *it2 << std::endl;
	}
}

std::vector<std::string> Location::getIndex() {
	std::vector<std::string>	   _stringIndex;
	std::vector<LineLoc>::iterator it = _locationLine.begin();
	while (it != _locationLine.end()) {
		if (it->getCmd() == "index") {
			_stringIndex = it->getArgs();
			return (_stringIndex);
		}
		it++;
	}
	return _stringIndex;
}
