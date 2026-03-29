#ifndef SERVER_HPP
#define SERVER_HPP
#include "Location.hpp"
#include "macroDef.hpp"
#include "utils.hpp"

enum methodAllowed {
	GET = 1,
	POST = 2,
	DELETE = 3,
	GETPOST = 4,
	GETPOSTDELETE = 5,
	GETDELETE = 6,
	POSTDELETE = 7,
};

enum class ParsingState { START, SERVER, LOCATION, ERROR };

typedef struct loc_parsing_t {
	bool	 open = false;
	int		 brackets = 0;
	LineList lines;
} LocParsing;

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
	void							   setAutoIndex(LineListIter it);
	void							   setAllowedMethod(LineListIter it);
	void							   setPort(LineListIter it);
	void							   setRoot(LineListIter it);
	void							   setDefaultPage(LineListIter it);
	void							   setServerName(LineListIter it);
	void							   setErrorPage(LineListIter it);
	void							   setMaxBodySize(LineListIter it);
	void							   addLocation(LineListIter it, LocParsing& loc);
	ParsingState					   parseStart(LineListIter list);
	ParsingState					   parseServer(LineListIter list, LocParsing& loc);
	ParsingState					   parseLocation(LineListIter list, LocParsing& loc);

   public:
	Server();
	Server(LineList servStrings);
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
	void								checkIfConform();
};

#endif
