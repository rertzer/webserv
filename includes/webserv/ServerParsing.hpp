#ifndef INCLUDES_WEBSERV_SERVERPARSING_HPP_
#define INCLUDES_WEBSERV_SERVERPARSING_HPP_

#include <string>
#include <vector>

#include "webserv/macroDef.hpp"

class ServerParsing {
 public:
  ServerParsing() : created(false), is_open(false), brackets(0) {}

  void       countBrackets(std::string const& line);
  statusCode parse(std::string const& line, std::vector<Server>& serv);
  bool       getOpen();

 private:
  statusCode parseOpen(std::string const& line, std::vector<Server>& serv);
  statusCode parseClose(std::string const& line);

  bool                     created;
  bool                     is_open;
  int                      brackets;
  std::vector<std::string> lines;
};

#endif  // INCLUDES_WEBSERV_SERVERPARSING_HPP_
