/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 15:05:31 by pjay              #+#    #+#             */
/*   Updated: 2023/09/19 10:54:04 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "macroDef.hpp"
#include "Server.hpp"
#include "Status.hpp"
#include "Response.hpp"
#include "DirListing.hpp"

std::string readSpecFile(std::string file)
{
	std::ifstream fileOp;
	fileOp.open(file.c_str());
	if (access(file.c_str(), F_OK) == -1)
	{
		throw (ErrorException(404));
	}
	if (access(file.c_str(), R_OK) == -1)
	{
		throw (ErrorException(403));
	}
	if (fileOp.is_open())
	{
		std::stringstream fileStr;
		fileStr << fileOp.rdbuf();
		fileOp.close();
		return(fileStr.str());
	}
	else
	{
		fileOp.close();
		throw (ErrorException(404));
	}
}

std::string intToString(int n)
{
	std::stringstream ss;
	ss << n;
	return (ss.str());
}

Server & findTheDefaultServ(std::vector<Server>& serv, int motherPort)
{
	std::vector<Server>::iterator it = serv.begin();

	while (it != serv.end())
	{
		if (it->getListenPort() == motherPort)
		{
			return (*it);
		}
		it++;

	}
	throw (ServerException());
	return (*(serv.begin()));
}

Server & findTheServ(Request& req, std::vector<Server> & serv, int motherPort)
{
	std::vector<Server>::iterator it = serv.begin();
	while (it != serv.end())
	{
		if (req.getField("Host") == it->getServName() + ":" + intToString(req.getPort()))
		{
			if (motherPort == it->getListenPort())
					return (*it);
		}
		it++;
	}
	;
	return (findTheDefaultServ(serv, motherPort));
}

Response createErrorPage(int codeErr, Server serv)
{

	ContentMap contentMap;
	std::string status = Status::getMsg(codeErr);
	std::string contentType = contentMap.getContentValue(serv.getErrorPage(intToString(codeErr)).substr(serv.getErrorPage(intToString(codeErr)).rfind(".") + 1, serv.getErrorPage(intToString(codeErr)).length()));
	std::string content;
	try
	{
		content = readSpecFile(serv.getRoot() + serv.getErrorPage(intToString(codeErr)));
	}
	catch(const std::exception& e)
	{
		content = "<html><body>File deleted</body></html>";
	}
	std::string contentLength = intToString(content.length());
	std::string connectionClose = "close";
	Response errResp(status, contentType, contentLength, connectionClose, content);
	return (errResp);
}

int checkAllowMethod(Location loc)
{
	std::vector<LineLoc> lineLoc = loc.getLocationLine();
	std::vector<LineLoc>::iterator it = lineLoc.begin();
	bool get = false;
	bool post = false;
	bool deleteMethod = false;
	bool found = false;
	while (it != lineLoc.end())
	{
		if (it->getCmd() == "allow_methods")
		{
			if (it->checkArgs("GET") == 1)
				get = true;
			if (it->checkArgs("POST") == 1)
				post = true;
			if (it->checkArgs("DELETE") == 1)
				deleteMethod = true;
			found = true;
		}
		it++;
	}
	if (found == true)
	{
		if (get && post && deleteMethod)
			return (GETPOSTDELETE);
		if (get && post)
			return (GETPOST);
		if (get && deleteMethod)
			return (GETDELETE);
		if (post && deleteMethod)
			return (POSTDELETE);
		if (get)
			return (GET);
		if (post)
			return (POST);
		if (deleteMethod)
			return (DELETE);
		if (!get && !post && !deleteMethod )
			return (GETPOSTDELETE);
	}
	return (-1);
}

int getAllowMethodsServer(std::string allowMethod)
{
	std::stringstream ss(allowMethod);
	std::string defaultStock;
	bool get = false;
	bool post = false;
	bool deleteMethod = false;

	while (getline(ss,defaultStock, ' '))
	{
		if (defaultStock == "GET")
			get = true;
		else if (defaultStock =="POST")
			post = true;
		else if (defaultStock == "DELETE")
			deleteMethod = true;
		else
		{
			std::cout << "Error in allowed method" << std::endl;
			throw (ServerException());
		}
	}
	if (get && post && deleteMethod)
	return (GETPOSTDELETE);
	if (get && post)
		return (GETPOST);
	if (get && deleteMethod)
		return (GETDELETE);
	if (post && deleteMethod)
		return (POSTDELETE);
	if (get)
		return (GET);
	if (post)
		return (POST);
	if (deleteMethod)
		return (DELETE);
	if (!get && !post && !deleteMethod)
		return (GETPOSTDELETE);
	return (-1);
}

int checkAutoIndex(Location loc)
{
	std::vector<LineLoc> lineLoc = loc.getLocationLine();
	std::vector<LineLoc>::iterator it = lineLoc.begin();
	while (it != lineLoc.end())
	{
		if (it->getCmd() == "autoindex")
		{
			if (it->getArgs()[0] == "on")
				return (1);
			else
				return (0);
		}
		it++;
	}
	return (-1);
}

int checkForRedirection(Location& loc)
{
	std::vector<LineLoc> lineLoc = loc.getLocationLine();
	std::vector<LineLoc>::iterator it = lineLoc.begin();
	while (it != lineLoc.end())
	{
		if (it->getCmd() == "return")
		{
			if (it->getArgs().size() >= 2)
				return (1);
		}
		it++;
	}
	return (0);
}

std::pair<std::string, std::string> RedirectTo(Location& loc)
{
	std::vector<LineLoc> lineLoc = loc.getLocationLine();
	std::vector<LineLoc>::iterator it = lineLoc.begin();
	std::pair<std::string, std::string> ret;
	while (it != lineLoc.end())
	{
		if (it->getCmd() == "return")
		{
			ret.first = it->getArgs()[0];
			ret.second = it->getArgs()[1];
			return (ret);
		}
		it++;
	}
	return (ret);
}

int	isThereAspecRoot(Location& loc)
{
	std::vector<LineLoc> lineLoc = loc.getLocationLine();
	std::vector<LineLoc>::iterator it = lineLoc.begin();
	while (it != lineLoc.end())
	{
		if (it->getCmd() == "root")
			return (1);
		it++;
	}
	return (0);
}

std::string getArgsLoc(Location& loc, std::string toFind)
{
	std::vector<LineLoc> lineLoc = loc.getLocationLine();
	std::vector<LineLoc>::iterator it = lineLoc.begin();
	while (it != lineLoc.end())
	{
		if (it->getCmd() == toFind)
			return (it->getArgs()[0]);
		it++;
	}
	return ("");
}

void printServ(Server& serv)
{
	std::cout << "Server name : " << serv.getServName() << std::endl;
	std::cout << "Server root : " << serv.getRoot() << std::endl;
	std::cout << "Server Auto index : " << serv.getAutoIndex() << std::endl;
	std::cout << "Server allowed method : " << serv.getAllowMethods() << std::endl;
	std::cout << "Listening on port : " << serv.getListenPort()  << std::endl;
	for (std::map<std::string, std::string>::iterator it = serv.getAllErrorPage().begin(); it != serv.getAllErrorPage().end(); it++)
		std::cout << it->first << " : " << it->second << std::endl;

	std::cout << "--------Location-------------" << std::endl;

	std::vector<Location> loc = serv.getAllLocation();
	for (std::vector<Location>::iterator it = loc.begin(); it != loc.end(); it++)
	{
		std::cout << "Location path : " << it->getLocationPath() << std::endl;
		std::cout << "Location line : " << std::endl;
		it->printLoc();
	}
	std::cout << "----------------------------------------" << std::endl;
}


std::pair<std::string, std::string> getExtension(Location loc)
{
	std::vector<LineLoc> lineLoc = loc.getLocationLine();
	std::vector<LineLoc>::iterator it = lineLoc.begin();
	std::pair<std::string, std::string > ret;
	while (it != lineLoc.end())
	{
		if (it->getCmd() == "extension")
			ret.first = it->getArgs()[0];
		if (it->getCmd() == "cgi_path")
			ret.second = it->getArgs()[0];
		it++;
	}
	return (ret);
}

int checkIfOnlyDigits(std::string str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		if (!isdigit(str[i]))
			return (-1);
	}
	return (0);
}

std::string getUploadPath(Location loc)
{
	std::vector<LineLoc> lineLoc = loc.getLocationLine();
	std::vector<LineLoc>::iterator it = lineLoc.begin();
	std::string ret;
	while (it != lineLoc.end())
	{
		if (it->getCmd() == "upload_path")
			ret = it->getArgs()[0];
		it++;
	}
	return (ret);
}
