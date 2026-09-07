#include <algorithm>
#include <cctype>
#include <locale>
#include <string>
#include <vector>

#include "webserv/macroDef.hpp"

std::vector<std::string> splitCsv(std::string const& str) {
  return splitCsv(str, ",");
}

std::vector<std::string> splitCsv(std::string const& str, std::string const& sep) {
  size_t                   start = 0;
  std::vector<std::string> splited;

  while (true) {
    auto        end = str.find(sep, start);
    std::string field = str.substr(start, end - start);
    stringTrim(field);
    splited.push_back(field);
    if (end == std::string::npos) {
      break;
    }
    start = end + 1;
  }
  return splited;
}

bool ciCompare(std::string const& left, std::string const& right) {
  std::string ci_left = lowString(left);
  std::string ci_right = lowString(right);
  if (ci_left.compare(ci_right)) {
    return false;
  }
  return true;
}

std::string lowString(std::string const& str) {
  std::string lowercase;
  std::transform(str.begin(), str.end(), lowercase.begin(), ::tolower);
  return lowercase;
}

std::string envFormat(std::string const& str) {
  std::locale loc;
  std::string ci_string;

  for (std::string::size_type i = 0; i < str.length(); ++i) {
    std::string tmp = "_";
    tmp[0] = std::toupper(str[i], loc);
    if (tmp[0] == '-')
      tmp[0] = '_';
    ci_string.append(tmp);
  }
  return ci_string;
}
