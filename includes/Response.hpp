#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include "ContentMap.hpp"
#include "FileDesc.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "macroDef.hpp"

class TCPSocket;
enum codeProb {
	OK = 200,
	FILE_NOT_FOUND = 404,
	ACCESS_DENIED = 403,
	METHOD_NOT_ALLOWED = 405,
};

class Response {
   private:
	Server								_serv;
	HttpMethod							method;
	std::string							_status;
	std::string							_contentType;
	std::string							_contentLength;
	std::string							_connectionClose;
	std::string							_content;
	std::vector<std::string>			_setCookie;
	std::pair<std::string, std::string> _extensionAllowed;
	int									_readFileAccess;
	std::string							_autoIndex;
	ContentMap							_contentMap;
	std::string							_location;
	std::string							_root;
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

	void setWithLocRoot(Location& loc);
	bool setWithLocRedirection(Location& loc, Request& req);
	int	 respWithoutLoc(Request& req);

   public:
	Response(Request& req, Server& serv);
	Response(std::string status,
			 std::string contentType,
			 std::string contentLength,
			 std::string connectionClose,
			 std::string content);
	Response(Response const& resp);
	Response&	operator=(Response const& rhs);
	std::string getResponse();
	int			respWithCgi(Request& req);
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
	void setContentTypeByRequest(Request const& req);
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
	std::string							getAutoIndex(void) const;
	std::pair<std::string, std::string> getExtensionAllowed(void) const;
	int									getReadFileAccess(void) const;
	ContentMap							getContentMap(void) const;
	BitSet								getAllowedMethods(void) const;
	bool								isAllowed(HttpMethod method) const;
	std::vector<std::string>			getCookie(void) const;
};

#endif
