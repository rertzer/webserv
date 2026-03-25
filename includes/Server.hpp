#ifndef SERVER_HPP
#define SERVER_HPP
#include "Location.hpp"
#include "macroDef.hpp"

enum methodAllowed {
	GET = 1,
	POST = 2,
	DELETE = 3,
	GETPOST = 4,
	GETPOSTDELETE = 5,
	GETDELETE = 6,
	POSTDELETE = 7,
};

class Server {
   private:
	std::string						   _servName;
	int								   _allowedMethod;
	std::string						   _autoIndex;
	int								   _nPort;
	std::vector<std::string>		   _defaultPage;
	std::string						   _root;
	std::map<std::string, std::string> _errorPage;
	std::vector<Location>			   _location;
	int								   _maxBodySize;

   public:
	Server();
	Server(std::vector<std::string> servStrings);
	Server(const Server& rhs);
	~Server();
	Server&								operator=(const Server& rhs);
	std::vector<std::string>&			getDefaultPage();
	std::string&						getServName();
	int									getBodySize() const;
	std::string&						getRoot();
	std::string							getErrorPage(std::string errorNb);
	std::map<std::string, std::string>& getAllErrorPage();
	std::vector<Location>&				getAllLocation();
	int&								getListenPort();
	int									getAllowMethods();
	std::string							getAutoIndex();
	void								checkIfHaveNeccessary();
};

#endif
