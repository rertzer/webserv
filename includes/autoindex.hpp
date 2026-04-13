#ifndef AUTOINDEX_HPP
#define AUTOINDEX_HPP

#include <string>

enum class AutoIndex { NONE, ON, OFF };

AutoIndex autoIndexFromString(std::string str);
#endif
