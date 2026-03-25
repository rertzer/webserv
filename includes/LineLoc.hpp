#ifndef LINELOC_HPP
#define LINELOC_HPP

#include <string>
#include <vector>

class LineLoc {
   private:
	std::string				 _cmd;
	std::vector<std::string> _args;

   public:
	std::string&			  getCmd();
	std::vector<std::string>& getArgs();
	LineLoc(std::string line);
	int checkArgs(std::string toFind);
};

#endif
