#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include "ContentMap.hpp"
#include "FileDesc.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "macroDef.hpp"

class Connection;
enum codeProb {
	OK = 200,
	FILE_NOT_FOUND = 404,
	ACCESS_DENIED = 403,
	METHOD_NOT_ALLOWED = 405,
};

class Response {
   private:
	Server&								_serv;
	HttpMethod							method;
	std::string							_status;
	std::string							_contentType;
	std::string							_contentLength;
	std::string							_connectionClose;
	std::string							_content;
	std::vector<std::string>			_setCookie;
	std::pair<std::string, std::string> _extensionAllowed;
	int									_readFileAccess;
	AutoIndex							_autoIndex;
	ContentMap							_contentMap;
	std::string							_location;
	std::string							_root;
	std::string							root_path;
	BitSet								allowed_methods;
	std::pair<std::string, std::string> extractField(size_t pos);
	void								extractFields();
	std::string appendDirContent(std::string content, FileDesc const& filedesc);
	std::string getResponseStatus() const;
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
	bool		setRequestQuery(Location& loc, Request& req);
	void		createAutoIndexResp(Request& req, Location loc);
	void		setWithLocRoot(Location& loc);
	bool		setWithLocRedirection(Location& loc, Request& req);
	int			respWithoutLoc(Request& req);
	void		fillPart(Request req);
	std::string readFile(std::string file);
	bool		testFileAccess(std::string file);

   public:
	Response(Request& req);
	Response(Server& serv, int errcode);
	Response(Response const& resp);
	Response&	operator=(Response const& rhs);
	std::string getResponse();
	int			respWithCgi(Request& req);
	void		setErrorPage(int errcode);
	// setter
	void setCookie(std::string ck);
	void Setserv(Server);
	void setMethod(HttpMethod);
	void setStatus(std::string);
	void setContentType(std::string);
	void setContentLength(std::string);
	void setConnectionClose(std::string);
	void setContent(std::string);
	void setContentWithLength(std::string);
	void setLocation(std::string);
	void setRoot(std::string);
	void setAutoIndex(std::string);
	void setExtensionAllowed(std::pair<std::string, std::string>);
	void setReadFileAccess(int);
	void setContentMap(ContentMap);
	void setAllowedMethods(BitSet);
	void fillOK(std::string content);
	// getter
	Server&								getServ(void);
	HttpMethod							getMethod(void) const;
	std::string							getStatus(void) const;
	std::string							getContentType(void) const;
	std::string							getContentLength(void) const;
	std::string							getConnectionClose(void) const;
	std::string							getContent(void) const;
	std::string							getDirContent(std::string path);
	std::string							getLocation(void) const;
	std::string							getRoot(void) const;
	AutoIndex							getAutoIndex(void) const;
	std::pair<std::string, std::string> getExtensionAllowed(void) const;
	int									getReadFileAccess(void) const;
	ContentMap							getContentMap(void) const;
	BitSet								getAllowedMethods(void) const;
	bool								isAllowed(HttpMethod method) const;
	std::vector<std::string>			getCookie(void) const;
	std::string							getSpecIndex(Location loc);
	Location							getTheLocation(std::string path);
	int									checkIfLocation(std::string path);
	std::string							getFilePath(std::string const& file) const;
};

#endif
