#include "autoindex.hpp"

AutoIndex autoIndexFromString(std::string str) {
	if (str == "on") {
		return AutoIndex::ON;
	} else if (str == "off") {
		return AutoIndex::OFF;
	}
	return AutoIndex::NONE;
}
