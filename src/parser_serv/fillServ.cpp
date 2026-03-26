#include <unordered_map>
#include <unordered_set>

#include "Server.hpp"

static void removeComments(std::string& line);
static bool blankOnly(std::string const& line);

void printVec(std::vector<std::string> vec) {
	for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
		std::cout << *it << std::endl;
}

bool checkPort(std::vector<Server>& serv) {
	std::unordered_map<int, std::unordered_set<std::string>> port_names;
	auto													 ok = true;

	for (auto it = serv.begin(); it != serv.end(); it++) {
		auto& names = port_names[it->getListenPort()];

		if (names.find(it->getServName()) != names.end()) {
			std::cout << "Servers with the same name must have different port numbers."
					  << std::endl;
			ok = false;
			break;
		} else {
			names.insert(it->getServName());
		}
	}
	return (ok);
}

bool fillServ(std::string av, std::vector<Server>& serv) {
	std::ifstream			 conf;
	std::vector<std::string> serv_strings;
	int						 open_brackets = 0;
	bool					 serv_open = false;

	try {
		conf.open(av.c_str(), std::fstream::in);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
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
		}
		if (serv_open == true) {
			if (line.find_first_not_of(" \t\n\r") != std::string::npos)
				serv_strings.push_back(line);
			if (line.find("{") != std::string::npos)
				++open_brackets;
			if (line.find("}") != std::string::npos)
				--open_brackets;
			if (open_brackets == 0) {
				serv_open = false;
				serv_strings.pop_back();
				serv.push_back(Server(serv_strings));
				serv_strings.clear();
			}
		}
		if (open_brackets < 0) {
			return (false);
		}
	}
	return (checkPort(serv));
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
