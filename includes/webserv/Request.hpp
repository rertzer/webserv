#ifndef INCLUDES_WEBSERV_REQUEST_HPP_
#define INCLUDES_WEBSERV_REQUEST_HPP_

#include <fcntl.h>
#include <exception>
#include <iostream>
#include <map>
#include <string>

#include "webserv/Cgi.hpp"
#include "webserv/Server.hpp"
#include "webserv/macroDef.hpp"

class Connection;

class Request {
 public:
  explicit Request(Connection* s);
  Request(Request const& rhs);
  ~Request();

  Request& operator=(Request const& rhs);

  int                                       getPort() const;
  CgiStatus                                 getCgiStatus() const;
  const std::string&                        getProtocol() const;
  HttpMethod                                getMethod() const;
  const std::string&                        getQuery() const;
  const std::map<std::string, std::string>& getHeader() const;
  const std::map<std::string, std::string>& getTrailer() const;
  const std::string&                        getContent() const;
  std::string                               getField(std::string const& name) const;
  unsigned int                              getBodySize() const;
  Cgi*                                      getCgi() const;
  std::optional<std::string>                getExtension() const;
  Server*                                   getServer();
  void                                      setBodySize(int bs);
  void                                      setUploadPath(std::string up);
  void                                      setKeepAlive(bool keep);
  void                                      initCgi(std::string root, Location& loc);
  bool                                      isUpload() const;
  void                                      uploadAll();
  void                                      setQuery(std::string const& query);
  void                                      setKeepAlive();
  void                                      setCgi(Cgi* c);
  bool                                      ready() const;
  void                                      feed();
  void                                      eraseContent(int size);
  void                                      printCleanRequest() const;

  class RequestException : public std::exception {
   public:
    virtual const char* what() const throw() { return ("Error: request parsing error"); }
  };

 private:
  // Request();
  std::string  getLine(std::string const& sep);
  std::string  getLine(std::string& data, std::string const& sep);
  std::string  getFileName();
  size_t       getContentLength() const;
  size_t       getExtensionEnd(size_t begin) const;
  void         upload(std::string& part);
  void         uploadFile(std::string const& filename, std::string const& part);
  void         checkValidFileName(std::string const& filename) const;
  void         setStartLine();
  void         setHeader();
  void         setFields();
  void         setContent();
  void         setContentByChunked();
  unsigned int readChunk();
  void         setTrailer();
  void         setContentByLength();
  void         setServer();
  void         addField(std::string const& field);
  void         checkStartLine() const;
  bool         checkField(std::string const& name, std::string const& value) const;
  void         checkHeader() const;
  bool         contentExist() const;
  void         addMultipart(std::string& line);
  void         appendHeader();
  void         updateHeader();

  int                                port;
  unsigned int                       body_size;
  Connection*                        connection;
  Cgi*                               cgi;
  std::map<std::string, std::string> header;
  std::map<std::string, std::string> trailer;
  std::map<std::string, std::string> multipart;
  HttpMethod                         method;
  std::string                        query;
  std::string                        protocol;
  std::string                        content;
  std::string                        upload_path;
  bool                               header_ok;
  bool                               content_ok;
  Server*                            server;
};

std::ostream& operator<<(std::ostream& ost, Request const& rhs);

#endif  // INCLUDES_WEBSERV_REQUEST_HPP_
