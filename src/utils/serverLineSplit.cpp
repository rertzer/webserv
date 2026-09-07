#include "webserv/utils.hpp"

LineList serverLineSplit(Line const& line) {
  LineList space_splited = split(line);
  LineList splited;
  for (auto& token : space_splited) {
    auto semicolon_splited = extract(token, ";");
    splited.insert(splited.end(), semicolon_splited.begin(), semicolon_splited.end());
  }

  return splited;
}
