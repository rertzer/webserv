#ifndef LINELOC_HPP
#define LINELOC_HPP

#include <string>
#include <vector>

#include "utils.hpp"

class LineLoc {
   private:
	std::string				 _cmd;
	std::vector<std::string> _args;

   public:
	std::string&			  getCmd();
	std::vector<std::string>& getArgs();
	LineLoc(LineList& list);
	int checkArgs(std::string toFind) const;
};

#endif
