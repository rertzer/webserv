#include <string>
#include <vector>

#include "webserv/Server.hpp"
#include "webserv/ServerParsing.hpp"

bool ServerParsing::getOpen() {
  return is_open;
}

statusCode ServerParsing::parse(std::string const& line, std::vector<Server>& serv) {
  statusCode status = statusCode::PARSING;

  if (is_open) {
    status = parseOpen(line, serv);
  } else {
    status = parseClose(line);
  }
  return status;
}

statusCode ServerParsing::parseOpen(std::string const& line, std::vector<Server>& serv) {
  statusCode status = statusCode::PARSING;
  countBrackets(line);

  if (brackets == 0) {
    is_open = false;
    serv.push_back(Server(lines));
    created = true;
    lines.clear();
    status = statusCode::OK;
  } else if (brackets < 0) {
    status = statusCode::PARSING;
  } else {
    lines.push_back(line);
    status = statusCode::OK;
  }
  return status;
}

statusCode ServerParsing::parseClose(std::string const& line) {
  statusCode status = statusCode::PARSING;

  if (line.find("server {") != std::string::npos) {
    is_open = true;
    ++brackets;
    status = statusCode::OK;
  }
  return status;
}

void ServerParsing::countBrackets(std::string const& line) {
  for (auto letter : line) {
    if (letter == '{') {
      ++brackets;
    } else if (letter == '}') {
      --brackets;
    }
    if (brackets < 0) {
      break;
    }
  }
}
