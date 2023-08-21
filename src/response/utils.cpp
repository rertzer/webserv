/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 15:05:31 by pjay              #+#    #+#             */
/*   Updated: 2023/08/21 13:02:20 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "macroDef.hpp"
#include "Server.hpp"
#include "Status.hpp"
#include "Response.hpp"

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

Server findTheServ(Request& req, std::vector<Server>& serv, int motherPort)
{
	std::vector<Server>::iterator it = serv.begin();
	if (req.getField("Host").empty())
		return (*(serv.begin()));
	while (it != serv.end())
	{
		std::cout << "Enter here = " << req.getField("Host") << "| My serv name is =  "<< it->getServName() + ":" + intToString(req.getPort()) << std::endl;
		if (req.getField("Host") == it->getServName() + ":" + intToString(req.getPort()))
		{
			if (it->getListenPort().size() > 1)
			{
				for (std::vector<int>::iterator it2 = it->getListenPort().begin(); it2 != it->getListenPort().end(); it2++)
				{
					if (motherPort == *it2)
						return (*it);
				}
			}
			else
			{
				if (motherPort == *it->getListenPort().begin())
						return (*it);
			}

		}
		it++;
	}
	return (*(serv.begin()));
}

Response createErrorPage(int codeErr, Server serv)
{
	std::cout << "codeErr = " << codeErr << std::endl;
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
	while (it != lineLoc.end())
	{
		std::cout << "cmd = " << it->getCmd() << std::endl;
		if (it->getCmd() == "allow_methods")
		{
			if (it->checkArgs("GET") == 1)
				get = true;
			if (it->checkArgs("POST") == 1)
				post = true;
			if (it->checkArgs("DELETE") == 1)
				deleteMethod = true;
		}
		it++;
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

bool checkAutoIndex(Location loc)
{
	std::vector<LineLoc> lineLoc = loc.getLocationLine();
	std::vector<LineLoc>::iterator it = lineLoc.begin();
	while (it != lineLoc.end())
	{
		std::cout << "cmd = " << it->getCmd() << std::endl;
		if (it->getCmd() == "autoindex")
		{
			if (it->checkArgs("on") == 1)
				return (true);
			else
				return (false);
		}
		it++;
	}
	return (false);
}

bool changeToIndex(Location loc, Request& req, std::string root)
{
	std::vector<LineLoc> lineLoc = loc.getLocationLine();
	std::vector<LineLoc>::iterator it = lineLoc.begin();
	while (it != lineLoc.end())
	{
		std::cout << "cmd = " << it->getCmd() << std::endl;
		if (it->getCmd() == "index")
		{
			std::vector<std::string> args = it->getArgs();
			std::cout << "Enter" << std::endl;
			for (std::vector<std::string>::iterator it2 = args.begin(); it2 != args.end(); it2++)
			{
				std::cout << "root + *it2" << root + *it2 << std::endl;
				if (access((root + *it2).c_str(), F_OK) != -1)
				{
					req.setQuery(*it2);
					return (true);
				}
			}
		}
		it++;
	}
	return (false);
}
