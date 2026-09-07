#include <charconv>
#include <sstream>

#include "webserv/HttpMethod.hpp"
#include "webserv/ServerException.hpp"
#include "webserv/macroDef.hpp"

BitSet getAllowMethodsServer(LineList const& list) {
  BitSet methods;
  for (auto it = std::next(list.begin()); it != list.end(); ++it) {
    HttpMethod method = stringToMethod(*it);
    if (method != NONE) {
      methods.addFlag(method);
    } else {
      throw(ServerException());
    }
  }

  return methods;
}

int checkIfOnlyDigits(std::string str) {
  for (auto l : str) {
    if (!isdigit(l))
      return 1;
  }
  return 0;
}

std::string join(LineList const& list) {
  std::ostringstream oss;

  auto it = list.begin();

  oss << *it++;

  for (; it != list.end(); ++it)
    oss << ' ' << *it;

  return oss.str();
}

std::optional<int> toInt(const std::string& str) {
  int value;
  auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), value);

  if (ec == std::errc() && ptr == str.data() + str.size())
    return value;

  return std::nullopt;
}
