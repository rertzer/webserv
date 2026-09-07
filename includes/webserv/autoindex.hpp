#ifndef INCLUDES_WEBSERV_AUTOINDEX_HPP_
#define INCLUDES_WEBSERV_AUTOINDEX_HPP_

#include <string>

enum class AutoIndex { NONE, ON, OFF };

AutoIndex     autoIndexFromString(std::string str);
std::ostream& operator<<(std::ostream& ost, AutoIndex const& rhs);
#endif  // INCLUDES_WEBSERV_AUTOINDEX_HPP_
