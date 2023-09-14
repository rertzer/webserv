/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macroDef.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 11:29:20 by pjay              #+#    #+#             */
/*   Updated: 2023/09/14 13:36:49 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <exception>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <cstring>
#include <cstddef>
#include <bits/stdc++.h>
#include <sys/wait.h>
#include "color.hpp"

#define CONF_FILE_NO_GOOD std::cout << "The conf file doesn't respect subject requirement" << std::endl;
#define CFNG_SMI_COLON std::cout << "A line in the conf File isn't ending with a semicolon" << std::endl;
#define CFNG_BRAC_OPEN std::cout << "A close bracket appear that isn't open anywhere" << std::endl;
#define CFNG_BRAC_CLOSE std::cout << "A bracket is not close in the conf file" << std::endl;

enum parsEnd {BRAC_OPEN, BRAC_CLOSE, SMI_COL, BAD_END};

class Server;
class Request;
class Response;
class Location;


int 									checkConfFile(std::string av);
int 									fillServ(std::string av , std::vector<Server>& serv);
int										serverRun(std::vector<Server>);
std::string 							sendResponse(Request& req, std::vector<Server>& serv);
void									stringTrim(std::string & str);
void									stringDoubleQuotTrim(std::string & str);
std::vector<std::string>				splitCsv(std::string const & str);
std::vector<std::string>				splitCsv(std::string const & str, std::string const & sep);
bool									ciCompare(std::string const & left, std::string const & right);
std::string								lowString(std::string const & str);
std::string								envFormat(std::string const & str);
std::string								intToString(int n);
Server &								findTheServ(Request& req, std::vector<Server> & serv, int motherPort);
std::string								readFile(std::string file);
Response								createErrorPage(int codeErr, Server serv);
int										checkAutoIndex(Location loc);
int										checkAllowMethod(Location loc);
bool									changeToIndex(Location loc, Request& req, std::string root);
std::string								dirContent(std::string root, std::string path);
int										checkForRedirection(Location& loc);
std::pair<std::string, std::string>		RedirectTo(Location& loc);
std::string								getArgsLoc(Location& loc, std::string toFind);
int										isThereAspecRoot(Location& loc);
int										getAllowMethodsServer(std::string allowMethod);
void									printServ(Server& serv);
std::pair<std::string, std::string>		getExtension(Location Loc);
int 									checkIfOnlyDigits(std::string str);
std::string 							getUploadPath(Location loc);
void 									dealWithGet(Request req, Response& rep);
void 									dealWithPost(Request req, Response& rep);
void 									dealWithDelete(Request req, Response& rep);
std::string								readFile(std::string file, Response& rep);
std::string								getContentKey(std::string value, Response& rep);
std::string								findLocation(std::string path, Response& rep);
int 									checkIfLocation(std::string path, Response& rep);
Location 								getTheLocation(std::string path, Response& rep);
void 									feelPart(Request req, Response& rep);
int 									respWithLoc(Request &req, Response& rep);
int										initCgi(Request & req, Location & loc, Response& rep);
int										respWithCgi(Request & req, Response& rep);
int 									respWithOutLoc(Request& req, Response& rep);
std::string								getSpecIndex(Location loc, Response& rep);
void 									createAutoIndexResp(Request& req, Location loc, Response& rep);
void									checkExec(std::string filePath);
int										respWithLoc(Request& req, Response &rep);
int										respWithOutLoc(Request& req, Response &rep);
void									printCleanRequest(Request req);
