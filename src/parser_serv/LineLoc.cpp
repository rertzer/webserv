#include <iostream>
#include <ranges>
#include <unordered_set>

#include "LineLoc.hpp"
#include "ServerException.hpp"
#include "utils.hpp"

LineLoc::LineLoc(LineList& list) {
	std::unordered_set<std::string> valid_cmds = {"root",		   "return",	 "autoindex",
												  "allow_methods", "index",		 "extension",
												  "cgi_path",	   "upload_path"};
	if (valid_cmds.find(list[0]) == valid_cmds.end() ||
		(list[0] != "return" && list[0] != "allow_methods" && list.size() != 2)) {
		std::cout << "Not a valid command\n";
		throw(ServerException());
	}
	_cmd = list[0];

	for (auto it : list | std::views::drop(1)) {
		_args.push_back(it);
	}
}

std::string& LineLoc::getCmd() {
	return _cmd;
}

std::vector<std::string>& LineLoc::getArgs() {
	return _args;
}

int LineLoc::checkArgs(std::string toFind) const {
	for (size_t i = 0; i < _args.size(); i++) {
		if (_args[i] == toFind)
			return (1);
	}
	return 0;
}
