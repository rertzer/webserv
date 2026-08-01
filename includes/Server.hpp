#ifndef SERVER_HPP
#define SERVER_HPP

#include <functional>
#include <map>
#include <optional>

#include "BitSet.hpp"
#include "HttpMethod.hpp"
#include "Location.hpp"
#include "macroDef.hpp"
#include "utils.hpp"

enum class ParsingState { START, SERVER, LOCATION, ERROR };
enum class HtmlCode { AUTOINDEX_HEADER, AUTOINDEX_CONTENT, AUTOINDEX_FOOTER };

typedef struct loc_parsing_t {
	bool	 open = false;
	int		 brackets = 0;
	LineList lines;
} LocParsing;

using LocationRefOpt = std::optional<std::reference_wrapper<Location>>;
using ServerParserHandler = ParsingState (Server::*)(LineList& list, LocParsing& loc);

class Server {
   private:
	std::string						_servName;
	BitSet							allowed_method;
	AutoIndex						_autoIndex;
	int								_nPort;
	std::vector<std::string>		_defaultPage;
	std::string						_root;
	std::map<int, std::string>		_errorPage;
	std::vector<Location>			_location;
	int								_maxBodySize;
	std::map<HtmlCode, std::string> html_code;
	ParsingState					setAutoIndex(LineList& list, LocParsing& loc);
	ParsingState					setAllowedMethod(LineList& list, LocParsing& loc);
	ParsingState					setPort(Line& line, LocParsing& loc);
	ParsingState					setRoot(LineList& list, LocParsing& loc);
	ParsingState					setDefaultPage(LineList& list, LocParsing& loc);
	ParsingState					setServerName(LineList& list, LocParsing& loc);
	ParsingState					setErrorPage(LineList& list, LocParsing& loc);
	ParsingState					setMaxBodySize(LineList& list, LocParsing& loc);
	ParsingState					parseLocation(LineList& list, LocParsing& loc);
	void							addLocation(LineList& list, LocParsing& loc);
	ParsingState					parseStart(LineList& list, LocParsing& loc);
	ParsingState					parseServer(LineList& list, LocParsing& loc);
	void							loadHtmlCode();
	LocationRefOpt					findLocationByPath(std::string path);

   public:
	Server();
	Server(LineList servStrings);
	Server(const Server& rhs);
	~Server();
	Server&						operator=(const Server& rhs);
	std::vector<std::string>&	getDefaultPage();
	std::string&				getServName();
	int							getBodySize() const;
	std::string&				getRoot();
	std::string					getErrorPage(int errorNb) const;
	std::map<int, std::string>& getAllErrorPage();
	std::vector<Location>&		getAllLocation();
	LocationRefOpt				findLocation(std::string path);
	int							getListenPort() const;
	BitSet						getAllowMethods() const;
	AutoIndex					getAutoIndex();
	void						checkIfConform();
	std::string					getHtmlCode(HtmlCode kind);
	void						addMethod();
	bool						isAllowed(HttpMethod method) const;
};

#endif
