#ifndef SERVERPARSING_HPP
#define SERVERPARSING_HPP

#include <string>
#include <vector>

struct ServerParsing {
   public:
	ServerParsing() : created(false), is_open(false), brackets(0) {}

	bool					 created;
	bool					 is_open;
	int						 brackets;
	std::vector<std::string> lines;
};

#endif
