#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "ContentMap.hpp"
#include "FileDesc.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "macroDef.hpp"
#include "HttpStatus.hpp"

class Connection;

class Response {
   private:
	Server&								_serv;
	HttpMethod							method;
	HttpStatus							status;
	std::string							_contentType;
	std::string							_contentLength;
	std::string							_connectionClose;
	std::string							_content;
	std::vector<std::string>			_setCookie;
	std::pair<std::string, std::string> _extensionAllowed;
	HttpStatus							readFileAccess;
	AutoIndex							_autoIndex;
	ContentMap							_contentMap;
	std::string							relocation;
	std::string							_root;
	std::string							root_path;
	BitSet								allowed_methods;
	LocationRefOpt						loc;

	std::pair<std::string, std::string> extractField(size_t pos);
	void								extractFields();
	std::string appendDirContent(std::string content, FileDesc const& filedesc);
	std::string	getResponseStatus() const;
	std::string getResponseHeader();
	std::string getResponseLocation() const;
	std::string getResponseConnection() const;
	std::string getResponseContentHeader() const;
	std::string getResponseCookies() const;
	std::string getResponseContent() const;
	void		logResponse(std::string resp) const;
	void		dealWithMethod(Request& req);
	void		dealWithDelete(Request& req);
	int			respWithLoc(Request& req);
	bool		setRequestQuery(Request& req);
	void		createAutoIndexResp(Request& req, Location loc);
	void		setWithLocRoot(Location& loc);
	bool		setWithLocRedirection(Location& loc, Request& req);
	int			respWithoutLoc(Request& req);
	void		fillPart(Request req);
	std::string readFile(std::string file);
	bool		testFileAccess(std::string file);
	void setLocation(std::string path);
	std::string			getSpecIndex();

   public:
	Response(Request& req);
	Response(Server& serv, HttpStatus errcode);
	Response(Response const& resp);
	Response&	operator=(Response const& rhs);
	std::string getResponse();
	int			respWithCgi(Request& req);
	void		setErrorPage(HttpStatus);
	// setter
	void setCookie(std::string ck);
	void Setserv(Server);
	void setMethod(HttpMethod);
	void setStatus(HttpStatus);
	void setContentType(std::string);
	void setContentLength(std::string);
	void setConnectionClose(std::string);
	void setContent(std::string);
	void setContentWithLength(std::string);
	void setRoot(std::string);
	void setAutoIndex(std::string);
	void setExtensionAllowed(std::pair<std::string, std::string>);
	void setReadFileAccess(HttpStatus);
	void setContentMap(ContentMap);
	void setAllowedMethods(BitSet);
	void fillOK(std::string content);
	// getter
	Server&								getServ(void);
	HttpMethod							getMethod(void) const;
	HttpStatus							getStatus(void) const;
	std::string							getContentType(void) const;
	std::string							getContentLength(void) const;
	std::string							getConnectionClose(void) const;
	std::string							getContent(void) const;
	std::string							getDirContent(std::string path);
	std::string							getRoot(void) const;
	std::pair<std::string, std::string> getExtensionAllowed(void) const;
	HttpStatus							getReadFileAccess(void) const;
	ContentMap							getContentMap(void) const;
	BitSet								getAllowedMethods(void) const;
	bool								isAllowed(HttpMethod method) const;
	std::vector<std::string>			getCookie(void) const;
	std::string							getFilePath(std::string const& file) const;
};

#endif
