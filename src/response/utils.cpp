/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/11 15:05:31 by pjay              #+#    #+#             */
/*   Updated: 2023/08/24 11:33:51 by pjay             ###   ########.fr       */
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

int checkForRedirection(Location& loc)
{
	std::vector<LineLoc> lineLoc = loc.getLocationLine();
	std::vector<LineLoc>::iterator it = lineLoc.begin();
	while (it != lineLoc.end())
	{
		std::cout << "cmd = " << it->getCmd() << std::endl;
		if (it->getCmd() == "return")
		{
			std::cout << "return = " << it->getArgs()[0] << std::endl;
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
	std::cout << "Enter in is there a spec root | size of loc.vec()" << lineLoc.size() << " name of loc" << loc.getLocationPath() << std::endl;
	while (it != lineLoc.end())
	{
		std::cout << " In is there a spec root = " << it->getCmd() << std::endl;
		if (it->getCmd() == "root")
			return (1);
		it++;
	}
	return (0);
}

std::string getSpecRoot(Location& loc)
{
	std::vector<LineLoc> lineLoc = loc.getLocationLine();
	std::vector<LineLoc>::iterator it = lineLoc.begin();
	while (it != lineLoc.end())
	{
		if (it->getCmd() == "root")
			return (it->getArgs()[0]);
		it++;
	}
	return ("");
}
