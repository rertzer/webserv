#ifndef MACRODEF_HPP
#define MACRODEF_HPP

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "BitSet.hpp"
#include "utils.hpp"

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

statusCode				 fillServ(std::string av, std::vector<Server>& serv);
std::string				 sendResponse(Request& req, std::vector<Server>& serv);
void					 stringTrim(std::string& str);
void					 stringDoubleQuotTrim(std::string& str);
std::vector<std::string> splitCsv(std::string const& str);
std::vector<std::string> splitCsv(std::string const& str, std::string const& sep);
bool					 ciCompare(std::string const& left, std::string const& right);
std::string				 lowString(std::string const& str);
std::string				 envFormat(std::string const& str);
std::string				 intToString(int n);
Server&					 findTheServ(Request& req, std::vector<Server>& serv, int listeningPort);
std::string				 readFile(std::string file);
Response				 createErrorPage(int codeErr, Server serv);
int						 checkAutoIndex(Location loc);
BitSet					 checkAllowMethod(Location loc);
bool					 changeToIndex(Location loc, Request& req, std::string root);
int						 checkForRedirection(Location& loc);
std::pair<std::string, std::string> RedirectTo(Location& loc);
std::string							getArgsLoc(Location& loc, std::string toFind);
int									isThereAspecRoot(Location& loc);
BitSet								getAllowMethodsServer(LineList const& list);
void								printServ(Server& serv);
std::pair<std::string, std::string> getExtension(Location Loc);
int									checkIfOnlyDigits(std::string str);
std::string							getUploadPath(Location loc);
void								dealWithGet(Request req, Response& rep);
void								dealWithPost(Request req, Response& rep);
void								dealWithDelete(Request req, Response& rep);
std::string							readFile(std::string file, Response& rep);
std::string							getContentKey(std::string value, Response& rep);
std::string							findLocation(std::string path, Response& rep);
int									checkIfLocation(std::string path, Response& rep);
Location							getTheLocation(std::string path, Response& rep);
void								fillPart(Request req, Response& rep);
int									respWithLoc(Request& req, Response& rep);
int									initCgi(Request& req, Location& loc, Response& rep);
int									respWithCgi(Request& req, Response& rep);
int									respWithOutLoc(Request& req, Response& rep);
std::string							getSpecIndex(Location loc, Response& rep);
void								createAutoIndexResp(Request& req, Location loc, Response& rep);
void								checkExec(std::string filePath);
int									respWithLoc(Request& req, Response& rep);
int									respWithOutLoc(Request& req, Response& rep);
void								printCleanRequest(Request req);
Server& findTheDefaultServ(std::vector<Server>& serv, int listeningPort);

#endif
