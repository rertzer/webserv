#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "Server.hpp"

static void removeComments(std::string& line);
static bool blankOnly(std::string const& line);
static int	countBrackets(std::string const& line);
static bool checkDuplicatedPortNames(std::vector<Server>& serv);

bool fillServ(std::string av, std::vector<Server>& serv) {
	std::ifstream			 conf;
	std::vector<std::string> serv_strings;
	auto					 open_brackets = 0;
	auto					 serv_open = false;
	auto					 server_created = false;
	try {
		conf.open(av.c_str(), std::fstream::in);
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (false);
	}

	for (std::string line; getline(conf, line);) {
		removeComments(line);
		if (blankOnly(line))
			continue;
		if (line.find("server {") != std::string::npos) {
			if (serv_open) {
				std::cerr << "Error: Server parsing error.\n";
				return (false);
			}
			serv_open = true;
			++open_brackets;
			continue;
		} else if (serv_open) {
			open_brackets += countBrackets(line);

			if (open_brackets == 0) {
				serv_open = false;
				serv.push_back(Server(serv_strings));
				server_created = true;
				serv_strings.clear();
			} else {
				serv_strings.push_back(line);
			}
		} else {
			std::cerr << "Error: Server parsing error.\n";
			return (false);
		}
		if (open_brackets < 0) {
			return (false);
		}
	}
	if (!server_created) {
		std::cerr << "Error: Server parsing error.\n";
		return (false);
	}
	return (checkDuplicatedPortNames(serv));
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
	return (count);
}

static bool checkDuplicatedPortNames(std::vector<Server>& serv) {
	std::unordered_map<int, std::unordered_set<std::string>> port_names;
	auto													 ok = true;

	for (auto it = serv.begin(); it != serv.end(); it++) {
		auto& names = port_names[it->getListenPort()];

		if (names.find(it->getServName()) == names.end()) {
			names.insert(it->getServName());
		} else {
			std::cerr << "Servers with the same name must have different port numbers.\n";
			ok = false;
			break;
		}
	}
	return (ok);
}
