/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   macroDef.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/26 11:29:20 by pjay              #+#    #+#             */
/*   Updated: 2023/08/17 11:14:13 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <fstream>
#include <exception>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <cstddef>
#include <bits/stdc++.h>
#include <sys/wait.h>


//CFNG stand for Conf File No Good
#define CONF_FILE_NO_GOOD std::cout << "The conf file doesn't respect subject requirement" << std::endl;
#define CFNG_SMI_COLON std::cout << "A line in the conf File isn't ending with a semicolon" << std::endl;
#define CFNG_BRAC_OPEN std::cout << "A close bracket appear that isn't open anywhere" << std::endl;
#define CFNG_BRAC_CLOSE std::cout << "A bracket is not close in the conf file" << std::endl;

enum parsEnd {BRAC_OPEN, BRAC_CLOSE, SMI_COL, BAD_END};

class Server;
class Request;
class Response;


int 						checkConfFile(std::string av);
int 						fillServ(std::string av , std::vector<Server>& serv);
int							serverRun(std::vector<Server>);
std::string 				sendResponse(Request& req, std::vector<Server>& serv);
void						stringTrim(std::string & str);
std::vector<std::string>	splitCsv(std::string const & str);
bool						ciCompare(std::string const & left, std::string const & right);
std::string					lowString(std::string const & str);
std::string					intToString(int n);
Server						findTheServ(Request& req, std::vector<Server>& serv, int motherPort);
std::string					readFile(std::string file);
Response					createErrorPage(int codeErr, Server serv);
