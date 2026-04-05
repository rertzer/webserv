#ifndef SERVERPARSING_HPP
#define SERVERPARSING_HPP

#include <string>
#include <vector>

#include "macroDef.hpp"

class ServerParsing {
   public:
	ServerParsing() : created(false), is_open(false), brackets(0) {}

	void	   countBrackets(std::string const& line);
	statusCode parse(std::string const& line, std::vector<Server>& serv);
	bool	   getOpen();

   private:
	statusCode parseOpen(std::string const& line, std::vector<Server>& serv);
	statusCode parseClose(std::string const& line);

	bool					 created;
	bool					 is_open;
	int						 brackets;
	std::vector<std::string> lines;
};

#endif
