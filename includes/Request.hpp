#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <fcntl.h>
#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Cgi.hpp"
#include "Server.hpp"
#include "macroDef.hpp"

class TCPSocket;

class Request {
   public:
	Request(TCPSocket* s, std::vector<Server>& serv);
	Request(Request const& rhs);
	~Request();

	Request& operator=(Request const& rhs);

	int										  getPort() const;
	int										  getStatus() const;
	CgiStatus								  getCgiStatus() const;
	const std::string&						  getProtocol() const;
	HttpMethod								  getMethod() const;
	TCPSocket*								  getSocket() const;
	const std::string&						  getQuery() const;
	const std::map<std::string, std::string>& getHeader() const;
	const std::map<std::string, std::string>& getTrailer() const;
	const std::string&						  getContent() const;
	std::string								  getField(std::string const& name) const;
	unsigned int							  getBodySize() const;
	Cgi*									  getCgi() const;
	std::optional<std::string>				  getExtension() const;
	size_t									  getExtensionEnd(size_t begin) const;
	void									  setBodySize(int bs);
	void									  setUploadPath(std::string up);
	void									  initCgi(std::string root, Location& loc);
	bool	checkField(std::string const& name, std::string const& value) const;
	bool	checkSubField(std::string const& name, std::string const& value) const;
	bool	isUpload() const;
	void	upload_all();
	size_t	getContentLength() const;
	void	addField(std::string const& field);
	void	setQuery(std::string const& query);
	void	setKeepAlive();
	void	setCgi(Cgi* c);
	bool	ready() const;
	void	feed(std::vector<Server> serv);
	void	eraseContent(int size);
	Server& findServ(std::vector<Server>& servers, int listeningPort);

	class RequestException : public std::exception {
	   public:
		virtual const char* what() const throw() { return ("Error: request parsing error"); }
	};

   private:
	// Request();
	std::string	 getLine(std::string const& sep);
	std::string	 getLine(std::string& data, std::string const& sep);
	std::string	 getFileName();
	void		 upload(std::string& part);
	void		 uploadFile(std::string const& filename, std::string const& part);
	void		 checkValidFileName(std::string const& filename) const;
	void		 setControlData();
	void		 setHeader(std::vector<Server> serv);
	void		 setFields();
	void		 setContent();
	void		 setContentByChunked();
	unsigned int readChunk();
	void		 setTrailer();
	void		 setContentByLength();
	void		 checkControlData() const;
	void		 checkHeader() const;
	bool		 contentExist() const;
	void		 addMultipart(std::string& line);

	int								   port;
	int								   status;
	unsigned int					   body_size;
	TCPSocket*						   soc;
	Cgi*							   cgi;
	std::map<std::string, std::string> header;
	std::map<std::string, std::string> trailer;
	std::map<std::string, std::string> multipart;
	HttpMethod						   method;
	std::string						   query;
	std::string						   protocol;
	std::string						   content;
	std::string						   upload_path;
	bool							   header_ok;
	bool							   content_ok;
};

std::ostream& operator<<(std::ostream& ost, Request const& rhs);

#endif
