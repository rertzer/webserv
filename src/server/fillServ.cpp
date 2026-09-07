#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "webserv/Server.hpp"
#include "webserv/ServerParsing.hpp"
#include "webserv/macroDef.hpp"

static void       removeComments(std::string& line);
static bool       blankOnly(std::string const& line);
static statusCode checkDuplicatedPortNames(std::vector<Server>& serv);

statusCode fillServ(std::string path, std::vector<Server>& serv) {
  ServerParsing parsing{};
  auto          status = statusCode::PARSING;
  std::ifstream conf(path, std::ios::binary);

  if (!conf) {
    throw std::runtime_error("Cannot open file: " + path);
  }

  for (std::string line; getline(conf, line);) {
    removeComments(line);
    if (blankOnly(line)) {
      continue;
    }
    if ((status = parsing.parse(line, serv)) != statusCode::OK) {
      break;
    }
  }

  if (status != statusCode::OK || parsing.getOpen()) {
    std::cerr << "Error: Server parsing error.\n";
    return statusCode::PARSING;
  }

  return checkDuplicatedPortNames(serv);
}

static void removeComments(std::string& line) {
  size_t hash_position = line.find("#");
  if (hash_position != std::string::npos) {
    line = line.substr(0, hash_position);
  }
}

static bool blankOnly(std::string const& line) {
  return (line.find_first_not_of(" \t\r") == std::string::npos);
}

static statusCode checkDuplicatedPortNames(std::vector<Server>& serv) {
  std::unordered_map<int, std::unordered_set<std::string>> port_names;
  auto                                                     status = statusCode::OK;

  for (auto it = serv.begin(); it != serv.end(); it++) {
    auto& names = port_names[it->getListenPort()];

    if (names.find(it->getServName()) == names.end()) {
      names.insert(it->getServName());
    } else {
      std::cerr << "Servers with the same name must have different port numbers.\n";
      status = statusCode::PARSING;
      break;
    }
  }
  return status;
}
