#include "autoindex.hpp"
#include <iostream>

AutoIndex autoIndexFromString(std::string str) {
	if (str == "on") {
		return AutoIndex::ON;
	} else if (str == "off") {
		return AutoIndex::OFF;
	}
	return AutoIndex::NONE;
}

std::ostream& operator<<(std::ostream& ost, AutoIndex const& rhs) {
	switch (rhs){
		case AutoIndex::NONE:
			ost << "None";
			break;
		case AutoIndex::ON:
			ost << "On";
			break;
		case AutoIndex::OFF:
			ost << "Off";
			break;
	}
	return ost;
}
