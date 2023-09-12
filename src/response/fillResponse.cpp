/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fillResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 10:40:13 by pjay              #+#    #+#             */
/*   Updated: 2023/09/12 13:55:05 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "TCPSocket.hpp"
#include "macroDef.hpp"

std::string readFile(std::string file, Response &rep)
{
	std::ifstream fileOp;
	fileOp.open(file.c_str());
	if (access(file.c_str(), F_OK) == -1)
	{
		rep.setReadFileAccess(FILE_NOT_FOUND);
		return ("404");
	}
	if (access(file.c_str(), R_OK) == -1)
	{
		rep.setReadFileAccess(ACCESS_DENIED);
		return ("403");
	}
	if (fileOp.is_open())
	{
		std::stringstream fileStr;
		fileStr << fileOp.rdbuf();
		fileOp.close();
		rep.setReadFileAccess(OK);
		return (fileStr.str());
	}
	else
	{
		fileOp.close();
		rep.setReadFileAccess(FILE_NOT_FOUND);
		return ("404");
	}
}

void feelPart(Request req, Response &rep)
{
	if (req.getQuery() == "/")
	{
		std::string fileStr;
		std::vector<std::string> defaultPage = rep.getServ().getDefaultPage();
		for (std::vector<std::string>::iterator it = defaultPage.begin(); it != defaultPage.end(); it++)
		{
			fileStr = readFile(rep.getRoot() + *it, rep);
			if (fileStr == "403" && rep.getReadFileAccess() == ACCESS_DENIED)
			{
				break;
			}
			if (fileStr != "404" && rep.getReadFileAccess() != FILE_NOT_FOUND)
			{
				rep.setContentType(rep.getContentMap().getContentValue(it->substr(it->rfind(".") + 1, it->length())));
				break;
			}
		}
		if (fileStr == "404" && rep.getReadFileAccess() == FILE_NOT_FOUND)
			rep = createErrorPage(404, rep.getServ());
		else if (fileStr == "403" && rep.getReadFileAccess() == ACCESS_DENIED)
			rep = createErrorPage(403, rep.getServ());
		else
		{
			rep.setStatus("200 OK");
			rep.setContent(fileStr);
			rep.setContentLength(intToString(rep.getContent().length()));
			rep.setConnectionClose("keep-alive");
		}
	}
	else
	{
		std::string	fileStr;
		fileStr = readFile(rep.getRoot() + req.getQuery(), rep);
		if (fileStr == "404" && rep.getReadFileAccess() == FILE_NOT_FOUND)
		{

			throw (ErrorException(404));
		}
		else if (fileStr == "403" && rep.getReadFileAccess() == ACCESS_DENIED)
			rep = createErrorPage(403, rep.getServ());
		else
		{
			rep.setStatus("200 OK");
			rep.setContentType(rep.getContentMap().getContentValue(req.getQuery().substr(req.getQuery().rfind(".") + 1, req.getQuery().length())));
			rep.setContent(fileStr);
			rep.setContentLength(intToString(rep.getContent().length()));
			rep.setConnectionClose("keep-alive");
		}
	}
}




void createAutoIndexResp(Request& req, Location loc, Response &rep) {

	int allowMethod = checkAllowMethod(loc);
	if (allowMethod != -1)
		rep.setAllowedMethods(allowMethod);
	if ((req.getMethod() == "GET" && (rep.getAllowedMethods() == GET || rep.getAllowedMethods() == GETPOST || rep.getAllowedMethods() == GETDELETE || rep.getAllowedMethods() == GETPOSTDELETE)) || \
		(req.getMethod() == "POST" && (rep.getAllowedMethods() == POST || rep.getAllowedMethods() == GETPOST || rep.getAllowedMethods() == POSTDELETE || rep.getAllowedMethods() == GETPOSTDELETE)))
	{
		rep.setContent(dirContent(rep.getRoot(), req.getQuery()));
		rep.setStatus("200");
		rep.setMethod(req.getMethod());
		rep.setContentType("text/html");
		rep.setContentLength(intToString(rep.getContent().length()));
	}
	else
	{
		rep = createErrorPage(405, rep.getServ());
	}

}

void	setCookie(std::string ck, Response &rep)
{
	rep.getCookie().push_back(ck);
}

int	initCgi(Request & req, Location & loc, Response &rep)
{
	Cgi *	myCgi = new Cgi(req.getMethod(), rep.getRoot() , req, getExtension(loc));
	req.setCgi(myCgi);
	return (0);
}

std::pair<std::string, std::string>	extractField(size_t pos, Response &rep)
{
	std::string	line;
	std::pair<std::string, std::string>	field;

	if (pos != std::string::npos)
	{
		line = rep.getContent().substr(0, pos);
		rep.getContent().erase(0, pos + 2);
		pos = line.find(":");
		if (pos != std::string::npos)
		{
			field.first = line.substr(0, pos);
			field.second = line.substr(pos + 1, -1);
		}
		stringTrim(field.first);
		stringTrim(field.second);
	}
	return (field);
}

