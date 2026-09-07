#ifndef INCLUDES_WEBSERV_MACRODEF_HPP_
#define INCLUDES_WEBSERV_MACRODEF_HPP_

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

#include "webserv/BitSet.hpp"
#include "webserv/utils.hpp"

#define CONF_FILE_NO_GOOD \
  std::cout << "The conf file doesn't respect subject requirement" << std::endl;
#define CFNG_SMI_COLON \
  std::cout << "A line in the conf File isn't ending with a semicolon" << std::endl;
#define CFNG_BRAC_OPEN std::cout << "A close bracket appear that isn't open anywhere" << std::endl;
#define CFNG_BRAC_CLOSE std::cout << "A bracket is not close in the conf file" << std::endl;

enum class statusCode {
  OK,
  INTERNAL,
  INVALID_ARG,
  PARSING,
  SERVER,
  CGI,
  SOCKET,
  POLLING,
  STANDARD
};

enum parsEnd { BRAC_OPEN, BRAC_CLOSE, SMI_COL, BAD_END };

class Server;
class Request;
class Response;
class Location;

template <typename E>
statusCode handleException(const E& e, statusCode code) {
  std::cerr << e.what() << std::endl;
  return code;
}

statusCode               fillServ(std::string av, std::vector<Server>& serv);
std::string              sendResponse(Request& req, std::vector<Server>& serv);
void                     stringTrim(std::string& str);
void                     stringDoubleQuotTrim(std::string& str);
std::vector<std::string> splitCsv(std::string const& str);
std::vector<std::string> splitCsv(std::string const& str, std::string const& sep);
bool                     ciCompare(std::string const& left, std::string const& right);
std::string              lowString(std::string const& str);
std::string              envFormat(std::string const& str);
bool                     changeToIndex(Location loc, Request& req, std::string root);
BitSet                   getAllowMethodsServer(LineList const& list);
int                      checkIfOnlyDigits(std::string str);
void                     dealWithGet(Request req, Response& rep);
void                     dealWithPost(Request req, Response& rep);
std::string              getContentKey(std::string value, Response& rep);
std::string              findLocation(std::string path, Response& rep);
int                      respWithCgi(Request& req, Response& rep);
int                      respWithOutLoc(Request& req, Response& rep);
std::string              extractDirPath(std::string path);

#endif  // INCLUDES_WEBSERV_MACRODEF_HPP_
