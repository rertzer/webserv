#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "Server.hpp"
#include "ServerParsing.hpp"
#include "macroDef.hpp"

static void		  removeComments(std::string& line);
static bool		  blankOnly(std::string const& line);
static int		  countBrackets(std::string const& line);
static statusCode checkDuplicatedPortNames(std::vector<Server>& serv);

statusCode fillServ(std::string path, std::vector<Server>& serv) {
	ServerParsing parsing{};

	std::ifstream conf(path, std::ios::binary);
	if (!conf) {
		throw std::runtime_error("Cannot open file: " + path);
	}

	for (std::string line; getline(conf, line);) {
		removeComments(line);
		if (blankOnly(line))
			continue;
		if (line.find("server {") != std::string::npos) {
			if (parsing.is_open) {
				std::cerr << "Error: Server parsing error.\n";
				return statusCode::PARSING;
			}
			parsing.is_open = true;
			++parsing.brackets;
			continue;
		} else if (parsing.is_open) {
			parsing.brackets += countBrackets(line);

			if (parsing.brackets == 0) {
				parsing.is_open = false;
				serv.push_back(Server(parsing.lines));
				parsing.created = true;
				parsing.lines.clear();
			} else {
				parsing.lines.push_back(line);
			}
		} else {
			std::cerr << "Error: Server parsing error.\n";
			return statusCode::PARSING;
		}
		if (parsing.brackets < 0) {
			return statusCode::PARSING;
		}
	}
	if (!parsing.created) {
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

static int countBrackets(std::string const& line) {
	int count = 0;
	if (line.find("{") != std::string::npos) {
		count = 1;
	}
	if (line.find("}") != std::string::npos) {
		count -= 1;
	}
	return count;
}

static statusCode checkDuplicatedPortNames(std::vector<Server>& serv) {
	std::unordered_map<int, std::unordered_set<std::string>> port_names;
	auto													 status = statusCode::OK;

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
