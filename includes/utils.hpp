#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

#define SPACES " \f\n\r\t\v"

using Line = std::string;
using LineList = std::vector<Line>;
using LineListIter = LineList::iterator;

LineList	split(Line const& line, std::string sep = SPACES);
LineList	extract(Line const& line, std::string sep = SPACES);
LineList	serverLineSplit(Line const& line);
void		printLineList(LineList const& list);
std::string join(LineList const& list);

#endif
