#ifndef UTILS_HPP
#define UTILS_HPP

#include <optional>
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
std::string getFileContent(std::string path);
void		replaceAll(std::string& str, std::string const& old_word, std::string const& new_word);
std::optional<int> toInt(const std::string& str);

#endif
