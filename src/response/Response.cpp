/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjay <pjay@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/02 14:49:31 by pjay              #+#    #+#             */
/*   Updated: 2023/08/11 15:25:02 by pjay             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
// GET /repertoire/page.html HTTP/1.1

//  std::string response = "HTTP/1.1 200 OK\r\n"
//                                "Content-Type: text/html\r\n"
//                                "Content-Length: 24\r\n"
//                                "\r\n"
//                                "<html><body>Hello</body></html>";


std::string Response::readFile(std::string file)
{
	std::ifstream fileOp;
	fileOp.open(file.c_str());
	std::cout << "File to open = " << file << std::endl;
	if (access(file.c_str(), F_OK) == -1)
	{
			_readFileAccess = FILE_NOT_FOUND;
			return ("404");
	}
	if (access(file.c_str(), R_OK) == -1)
	{
			_readFileAccess = ACCESS_DENIED;
			return ("403");
	}
	if (fileOp.is_open())
	{
		std::cout << "Enterrqrwlpeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee" << std::endl;
		std::string stocked;
		std::string fileStr;
		while (getline(fileOp, stocked))
		{
			fileStr += stocked;
		}
		fileOp.close();
		return(fileStr);
	}
	else
	{
		fileOp.close();
		_readFileAccess = FILE_NOT_FOUND;
		return ("404");
	}
}

//We need to add something to find default server
//First : We find the hostname corresponding to the request
//Second : We find the port corresponding to the request
//if he match both we return the server
Server findTheServ(Request& req, std::vector<Server>& serv, int motherPort)
{
	std::vector<Server>::iterator it = serv.begin();
	while (it != serv.end())
	{
		if (req.getField("Host") == it->getServName())
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

void Response::CreateErrorPage(Request req, int codeErr)
{
	switch (codeErr)
	{
		case 404 :
			_status = "404 Not Found";
			_contentType = req.getField("Accept").substr(0, req.getField("Accept").find(","));
			_content = readFile(_serv.getRoot() + _serv.getErrorPage("404"));
			_contentLength = intToString(_content.length());
			break;
		case 403 :
			_status = "403 Access denied";
			_contentType = req.getField("Accept").substr(0, req.getField("Accept").find(","));
			_content = readFile(_serv.getRoot() + _serv.getErrorPage("403"));
			_contentLength = intToString(_content.length());
			break;
		default : // pop this part just didn t know default
			_status = "404 Not Found";
			_contentType = req.getField("Accept").substr(0, req.getField("Accept").find(","));
			_content = readFile(_serv.getRoot() + _serv.getErrorPage("404"));
			_contentLength = intToString(_content.length());
	}


}

void Response::dealWithGet(Request req)
{
	_method = "GET";
	if (req.getQuery() == "/")
	{
		std::string fileStr;
		for (std::vector<std::string>::iterator it = _serv.getDefaultPage().begin(); it != _serv.getDefaultPage().end(); it++)
		{
			fileStr = readFile(_serv.getRoot() + *it);
			if (fileStr == "403" && _readFileAccess == ACCESS_DENIED)
				break;
			if (fileStr != "404" && _readFileAccess != FILE_NOT_FOUND)
				break;
		}
		//std::cout << "file found = " << fileStr << std::endl;
		if (fileStr == "404" && _readFileAccess == FILE_NOT_FOUND)
			CreateErrorPage(req, 404);
		else if (fileStr == "403" && _readFileAccess == ACCESS_DENIED)
			CreateErrorPage(req, 403);
		else
		{
			std::cout << "Accept field = " << req.getField("Accept") << std::endl;
			_status = "200 OK";
			_contentType = req.getField("Accept").substr(0, req.getField("Accept").find(","));
			_content = fileStr;
			_contentLength = intToString(_content.length()); // mettre en string
		}
	}
	else
	{
		//std::cout << "_serv.getRoot() + req.getQuery() = " << _serv.getRoot() + req.getQuery() << std::endl;
		std::string fileStr = readFile(_serv.getRoot() + req.getQuery());
		//std::cout << "Content that is not root " << fileStr << std::endl;
		if (fileStr == "404" && _readFileAccess == FILE_NOT_FOUND)
			CreateErrorPage(req, 404);
		else if (fileStr == "403" && _readFileAccess == ACCESS_DENIED)
			CreateErrorPage(req, 403);
		else
		{
			_status = "200 OK";
			_contentType = req.getField("Accept").substr(0, req.getField("Accept").find(","));
			_content = fileStr;
			_contentLength = intToString(_content.length());
		//	std::cout << "Content that is not root " << fileStr << std::endl;
		}
	}
}

std::string Response::getResponse()
{
	std::string response = "HTTP/1.1 " + _status + " \r\n"
							"Content-Type: " + _contentType + "\r\n"
							"Content-Length: " + _contentLength + "\r\n"
							"\r\n"
							+ _content;
	return (response);
}

Response::Response(Request& req, std::vector<Server> serv, int motherPort)
{
	_readFileAccess = OK;
	_serv = findTheServ(req, serv, motherPort);
	if (req.getMethod() == "GET")
	{
		dealWithGet(req);
	}
}

